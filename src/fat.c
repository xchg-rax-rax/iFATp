#include "fat.h"

#include <stdio.h>


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
    // also sus pointer arithmetic
    uint16_t table_value = *(uint16_t*)&target_fat[entry_offset];

    if (active_cluster & 0x0001) {
        return table_value >> 4;
    }
    else {
        return table_value & 0x0FFF;
    }
}

uint8_t* get_root_directory_ptr(uint8_t* file_system) {
}
