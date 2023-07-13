#include "fat.h"

#include <stdio.h>
#include <stdlib.h>

#include "util.h"

BPBComputedValues_t compute_values(BS_FAT* bs) {
    BPBComputedValues_t computed_values;
    if(bs->bpb.num_bytes_per_sector == 0) {
        computed_values.fs_type = exFAT;
        return computed_values;
    }
    computed_values.total_sectors = (bs->bpb.num_sectors_in_logical_volume == 0) ? bs->bpb.large_sector_count : bs->bpb.num_sectors_in_logical_volume;
    computed_values.num_sectors_per_fat = (bs->bpb.num_sectors_per_fat == 0) ? bs->ebr.fat32.num_sectors_per_fat : bs->bpb.num_sectors_per_fat;
    computed_values.num_root_dir_sectors = ((bs->bpb.num_root_directory_entires * 32) + (bs->bpb.num_bytes_per_sector - 1)) / bs->bpb.num_bytes_per_sector;
    computed_values.first_data_sector_index = bs->bpb.num_reserved_sectors + (bs->bpb.num_file_allocation_tables * computed_values.num_sectors_per_fat) + computed_values.num_root_dir_sectors;
    computed_values.first_root_dir_entry_sector_index = (bs->bpb.num_reserved_sectors + (computed_values.num_sectors_per_fat * bs->bpb.num_file_allocation_tables));
    computed_values.first_fat_sector_index = bs->bpb.num_reserved_sectors;
    computed_values.num_data_sectors = computed_values.total_sectors - (bs->bpb.num_reserved_sectors + (bs->bpb.num_file_allocation_tables * computed_values.num_sectors_per_fat) + computed_values.num_root_dir_sectors);
    computed_values.num_clusters = computed_values.num_data_sectors / bs->bpb.num_sectors_per_cluster;
    determine_fs_type(&computed_values, bs);
    return computed_values;
}

void determine_fs_type(BPBComputedValues_t* computed_values, BS_FAT* bs) {
    FSType fs_type;
    if(bs->bpb.num_bytes_per_sector == 0) {
        fs_type = exFAT;
    }
    else if (computed_values->num_clusters < 4086) {
        fs_type = FAT12; 
    }
    else if (computed_values->num_clusters < 65525) {
        fs_type = FAT16; 
    }
    else if(bs->bpb.num_root_directory_entires == 0) {
        fs_type = FAT32; 
    }
    else {
        fs_type = UNKNOWN;
    }
    computed_values->fs_type = fs_type;
}

inline uint32_t get_num_sectors_in_logical_volume(BPB* bpb) {
    if (bpb->num_sectors_in_logical_volume == 0) {
        return bpb->large_sector_count;
    }
    return bpb->num_sectors_in_logical_volume;
}

uint16_t get_next_cluster_fat12(BPB* bpb, uint8_t* file_system, uint16_t active_cluster) {
    // Check that there even is a next cluster
    if (active_cluster == 0 || active_cluster >= 0xFF7) {
        return active_cluster;
    }

    // FAT12 file table entries are 12 bytes so we need to do some 
    // arithmetic contortion to get this to work...
    uint16_t fat_offset = active_cluster + (active_cluster / 2);
    // doubt next line
    uint16_t fat_sector = bpb->num_reserved_sectors + (fat_offset / bpb->num_bytes_per_sector);
    uint16_t entry_offset = fat_offset % bpb->num_bytes_per_sector;

    uint8_t* target_fat = file_system + (bpb->num_bytes_per_sector * fat_sector);

    // no size checking on the fs, probably should do this somewhere... [SEC]
    // also such pointer arithmetic
    uint16_t table_value = *(uint16_t*)&target_fat[entry_offset];

    if (active_cluster & 0x0001) {
        return table_value >> 4;
    }
    else {
        return table_value & 0x0FFF;
    }
}

DIR_FAT_8_3_t* get_root_directory_ptr(uint8_t* file_system) {
    BS_FAT* bs = (BS_FAT*) file_system;
    BPBComputedValues_t computed_values = compute_values(bs);
    return (DIR_FAT_8_3_t*) (file_system + (computed_values.first_root_dir_entry_sector_index * bs->bpb.num_bytes_per_sector));
}

DIR_FAT_8_3_t* get_dir_fat_8_3(uint8_t* dir_ent_ptr) {
    // We don't care about the long file name of the directory (for now)
    // so we skip over any long file name entries.
    // We don't want to go on forever so we stop and return null if we
    // see 20 if blocks in a row
    int i = 0;
    for(; i < 20; i++) {
        DIR_FAT_LONG_FILENAME_t* lf_dir = (DIR_FAT_LONG_FILENAME_t*) dir_ent_ptr;
        if (lf_dir->lf_attribute != LF_ATTR) {
            break;
        }
        dir_ent_ptr += sizeof(DIR_FAT_LONG_FILENAME_t);
    }
    if (i == 20) {
        return NULL;
    }
    return (DIR_FAT_8_3_t*) dir_ent_ptr;
}

DIR_FAT_8_3_t** parse_root_directory(uint8_t* file_system, size_t* root_dir_ent_count) {
    DIR_FAT_8_3_t* root_dir_start = get_root_directory_ptr(file_system);
    DIR_FAT_8_3_t* root_dir_ent = root_dir_start; 
    *root_dir_ent_count = 0;
    while(((uint8_t*)root_dir_ent)[0] != 0x00) {
        if (root_dir_ent->attributes != 0x0F && 
            ((uint8_t*)root_dir_ent)[0] != 0xE5) {
            (*root_dir_ent_count)++;
        }
        root_dir_ent++;
    }
    if (*root_dir_ent_count == 0)  {
        return NULL;
    }
    DIR_FAT_8_3_t** entries = (DIR_FAT_8_3_t**) calloc(*root_dir_ent_count, sizeof(DIR_FAT_8_3_t*));

    if (entries == NULL) {
        printf("%s\n","[!] Failed to allocate array for root directory entries"); 
        return NULL;
    }

    root_dir_ent = root_dir_start; 
    int i = 0;
    while(((uint8_t*)root_dir_ent)[0] != 0x00) {
        if (root_dir_ent->attributes != 0x0F && 
            ((uint8_t*)root_dir_ent)[0] != 0xE5) {
            entries[i] = root_dir_ent;
            i++;
        }
        root_dir_ent++;
    }
    return entries;
}

/*
 *  Parses a directory at a given cluster, returns and array of pointers
 *  to the directory entries in that directory
 */
DIR_FAT_8_3_t** parse_directory(uint8_t* file_system, uint32_t cluster_number) {
    BS_FAT* bs = (BS_FAT*) file_system;
    
}
