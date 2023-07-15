#include <stdio.h>
#include <string.h>

#include "util.h"


void print_usage(char* bin) {
    printf("%s <path_to_disk_image>\n", bin);
}

void print_hex_dump(uint8_t* bytes, uint32_t size) {
    int i;
    for (i = 0; i < size; i++) {
        printf("%02X ", bytes[i]);
        if (i%16 == 15) {
            putchar('\n');
        }
    }
    if (i%16  != 0) {
        putchar('\n');
    };
}

void print_bios_parameter_block(BPB* bpb) {
    printf("%s","Entry Point: ");
    print_hex_dump(bpb->entry_point, 3);

    // string is not null terminated so we need to make a copy that is
    // probably should make a function/macro to do this
    char oem_identifier[9];
    memcpy(oem_identifier, bpb->oem_identifier, 8);
    oem_identifier[8] = '\0';
    printf("OEM Identifier: %s\n", oem_identifier);

    printf("Bytes Per Sector: %u\n", bpb->num_bytes_per_sector);

    printf("Sectors Per Cluster: %u\n", bpb->num_sectors_per_cluster);

    printf("Number of Reserved Sectors: %u\n", bpb->num_reserved_sectors);
    printf("Number of File Allocation Table: %u\n", bpb->num_file_allocation_tables);
    printf("Number of Root Directory Entries: %u\n", bpb->num_root_directory_entires);
    uint32_t num_sectors_in_logical_volume = get_num_sectors_in_logical_volume(bpb);
    printf("Sectors Per Logical Volume: %u\n", num_sectors_in_logical_volume);
    printf("Media Description Type: %X\n", bpb->media_description_type);
    printf("Sectors Per File Allocation Tables (FAT12/16): %u\n", bpb->num_sectors_per_fat);
    printf("Sectors Per Track: %u\n", bpb->num_sectors_per_track);
    printf("Number of Heads: %u\n", bpb->num_heads);
    printf("Number of Hidden Sectors: %u\n", bpb->num_hidden_sectors);
}

void print_extended_boot_record_fat12(EBR_FAT12* ebr) {
    printf("Drive Number: %u\n", ebr->drive_number);
    printf("NT Flags: %X\n", ebr->nt_flags);
    printf("Signature: %X\n", ebr->signature);
    printf("Volume ID: %X\n", ebr->volume_id);
    char volume_label[12];
    memcpy(volume_label, ebr->volume_label, 11);
    volume_label[11] = '\0';
    printf("Volume Label: %s\n", volume_label);
    char system_identifier_string[9];
    memcpy(system_identifier_string, ebr->system_identifier_string, 8);
    system_identifier_string[8] = '\0';
    printf("System Identifier String: %s\n", system_identifier_string);
    printf("%s","Boot Code:\n");
    print_hex_dump(ebr->boot_code, sizeof(ebr->boot_code));
    printf("%s","Boot Partition Signature: ");
    print_hex_dump((uint8_t*)&(ebr->boot_partition_signature), sizeof(ebr->boot_partition_signature));
}

void print_extended_boot_record_fat16(EBR_FAT16* ebr) {
    print_extended_boot_record_fat12((EBR_FAT12*) ebr);
}

void print_extended_boot_record_fat32(EBR_FAT32* ebr) {
    printf("Sectors Per File Allocation Table (FAT32): %u\n", ebr->num_sectors_per_fat);
    printf("NT Flags: %04X\n", ebr->flags);
    printf("FAT Version Number: %04X\n", ebr->fat_version_number);
    printf("Root Directory Cluster Number: %u\n", ebr->root_directory_cluster_number);
    printf("FSInfo Sector Number: %u\n", ebr->fsinfo_sector_number);
    printf("%s","Reserved Bytes:\n");
    print_hex_dump(ebr->reserved, sizeof(ebr->reserved));
    printf("Drive Number: %u\n", ebr->drive_number);
    printf("NT Flags: %02X\n", ebr->nt_flags);
    printf("Signature: %02X\n", ebr->signature);
    printf("Volume ID: %08X\n", ebr->volume_id);
    char volume_label[sizeof(ebr->volume_label)+1];
    memcpy(volume_label, ebr->volume_label, sizeof(ebr->volume_label));
    volume_label[sizeof(ebr->volume_label)] = '\0';
    printf("Volume Label: %s\n", volume_label);
    char system_identifier_string[sizeof(ebr->system_identifier_string)+1];
    memcpy(system_identifier_string, ebr->system_identifier_string, sizeof(ebr->system_identifier_string));
    system_identifier_string[sizeof(ebr->system_identifier_string)] = '\0';
    printf("System Identifier String: %s\n", system_identifier_string);
    printf("%s","Boot Code:\n");
    print_hex_dump(ebr->boot_code, sizeof(ebr->boot_code));
    printf("%s","Boot Partition Signature: ");
    print_hex_dump((uint8_t*)&(ebr->boot_partition_signature), sizeof(ebr->boot_partition_signature));
}

void print_fat_12_metadata(BS_FAT* bs) {
    printf("%s","\nBIOS Parameter Block\n");
    printf("%s","--------------------\n\n");
    print_bios_parameter_block(&(bs->bpb));
    printf("%s","\nExtended Boot Record\n");
    printf("%s","--------------------\n\n");
    print_extended_boot_record_fat12(&(bs->ebr.fat12));
}

void print_fat_16_metadata(BS_FAT* bs) {
    printf("%s","\nBIOS Parameter Block\n");
    printf("%s","--------------------\n\n");
    print_bios_parameter_block(&(bs->bpb));
    printf("%s","\nExtended Boot Record\n");
    printf("%s","--------------------\n\n");
    print_extended_boot_record_fat16(&(bs->ebr.fat16));
}

void print_fat_32_metadata(BS_FAT* bs) {
    printf("%s","\nBIOS Parameter Block\n");
    printf("%s","--------------------\n\n");
    print_bios_parameter_block(&(bs->bpb));
    printf("%s","\nExtended Boot Record\n");
    printf("%s","--------------------\n\n");
    print_extended_boot_record_fat32(&(bs->ebr.fat32));
}

void print_exfat_metadata(BS_exFAT* bs) {
    printf("%s", "Entry Point: ");
    print_hex_dump(bs->entry_point, sizeof(bs->entry_point));
    char file_system_name[sizeof(bs->file_system_name)+1];
    memcpy(file_system_name, bs->file_system_name, sizeof(bs->file_system_name));
    file_system_name[sizeof(bs->file_system_name)] = '\0';
    printf("File System Name: %s\n", file_system_name);
    printf("%s","Zero Reserved Bytes:\n");
    print_hex_dump(bs->zero_reserved_bytes, sizeof(bs->zero_reserved_bytes));
    printf("Partition Offset: %lu\n", bs->partition_offset);
    printf("Volume Length: %lu\n", bs->voulme_length);
    printf("FAT Offset: %u\n", bs->fat_offset);
    printf("FAT Length: %u\n", bs->fat_length);
    printf("Cluster Heap Offset: %u\n", bs->cluster_heap_offset);
    printf("Cluster Count: %u\n", bs->cluster_count);
    printf("First Cluster of Root Directory: %u\n", bs->first_cluster_of_root_directory);
    printf("Volume Serial Number: %08X\n", bs->volume_serial_number);
    printf("File System Revision: %u\n", bs->file_system_revision);
    printf("Volume Flags: %04X\n", bs->volume_flags);
    printf("Bytes Per Sector Shift: %u\n", bs->bytes_per_sector_shift);
    printf("Sectors Per Cluster Shift: %u\n", bs->sectors_per_cluster_shift);
    printf("Number of File Allocation Tables: %u\n", bs->num_file_allocation_tables);
    printf("Drive Select: %u\n", bs->num_file_allocation_tables);
    printf("Percentage Used: %u\n", bs->percentage_in_use);
    printf("%s","Reserved Bytes:\n");
    print_hex_dump(bs->reserved_bytes, sizeof(bs->reserved_bytes));
    printf("%s","Boot Code:\n");
    print_hex_dump(bs->boot_code, sizeof(bs->boot_code));
    printf("%s","Boot Partition Signature: ");
    print_hex_dump((uint8_t*)&(bs->boot_partition_signature), sizeof(bs->boot_partition_signature));
}

void print_computed_values(BPBComputedValues_t* computed_values) {
    printf("%s","\nBoot Sector Computed Values\n");
    printf("%s","---------------------------\n\n");
    printf("Total Sectors: %u\n", computed_values->total_sectors);
    printf("Sectors per FAT: %u\n", computed_values->num_sectors_per_fat);
    printf("Sectors in root directory: %u\n", computed_values->num_root_dir_sectors);
    printf("First FAT sector index: %u\n", computed_values->first_fat_sector_index);
    printf("First data sector index: %u\n", computed_values->first_data_sector_index);
    printf("Number of Data Sectors: %u\n", computed_values->num_data_sectors);
    printf("Number of Clusters: %u\n", computed_values->num_clusters);
    printf("%s", "Filesystem Type: ");
    print_fs_type(computed_values->fs_type);
}

void print_fs_type(FSType fs_type) {
    switch(fs_type) {
        case FAT12:
            printf("%s","FAT12\n");
            break;
        case FAT16:
            printf("%s","FAT16\n");
            break;
        case FAT32:
            printf("%s","FAT\n");
            break;
        case exFAT:
            printf("%s","exFAT\n");
            break;
        case NTFS:
            printf("%s","NTFS\n");
            break;
        case UNKNOWN:
        default:
            printf("%s","UNKNOWN\n");
            break;
    }
}

void print_dir_fat_8_3(DIR_FAT_8_3_t* dir_ent_ptr) {
    printf("Filename: %11s\n", dir_ent_ptr->file_name);
    // Modify this to print attributes that are present
    printf("Attributes: %02X\n", dir_ent_ptr->attributes);
    printf("NT Reserved: %02X\n", dir_ent_ptr->nt_reserved);
    // Modify to parse the date and time
    printf("Creation time (100ms): %02X\n", dir_ent_ptr->creation_time_tenths_sec);
    printf("Creation time (s,m,h): %04X\n", dir_ent_ptr->creation_time);
    printf("Creation date (d,m,y): %04X\n", dir_ent_ptr->creation_date);
    printf("Last access date (d,m,y): %04X\n", dir_ent_ptr->last_access_date);
    printf("First cluster number upper: %04X\n", dir_ent_ptr->first_cluster_number_upper);
    printf("Last modified time (s,m,h): %04X\n", dir_ent_ptr->creation_time);
    printf("Last modified date (d,m,y): %04X\n", dir_ent_ptr->last_access_date);
    printf("First cluster number lower: %04X\n", dir_ent_ptr->first_cluster_number_lower);
    printf("Size: %u bytes\n", dir_ent_ptr->size);
}

void print_dir_fat_8_3_pretty(DIR_FAT_8_3_t* dir_ent_ptr) {
    printf("Filename: %11s\n", dir_ent_ptr->file_name);
    // Modify this to print attributes that are present
    printf("%s", "Attributes: ");
    print_dir_fat_8_3_attrs(dir_ent_ptr->attributes);
    // Modify to parse the date and time
    printf("\n%s","Created: ");
    print_dir_fat_8_3_time(parse_fat_8_3_time(dir_ent_ptr->creation_time));
    putchar(' ');
    print_dir_fat_8_3_date(parse_fat_8_3_date(dir_ent_ptr->creation_date));
    printf("\n%s","Last Accessed: ");
    print_dir_fat_8_3_date(parse_fat_8_3_date(dir_ent_ptr->last_access_date));
    printf("\n%s","Last Modified time: ");
    print_dir_fat_8_3_time(parse_fat_8_3_time(dir_ent_ptr->last_modified_time));
    putchar(' ');
    print_dir_fat_8_3_date(parse_fat_8_3_date(dir_ent_ptr->last_modified_date));
    printf("\nSize: %u bytes\n", dir_ent_ptr->size);
}

void print_dir_fat_8_3_short(DIR_FAT_8_3_t* dir_ent_ptr) {
    printf("%11s\t", dir_ent_ptr->file_name);
    // Modify this to print attributes that are present
    print_dir_fat_8_3_attrs(dir_ent_ptr->attributes);
    // Modify to parse the date and time
    putchar('\t');
    print_dir_fat_8_3_time(parse_fat_8_3_time(dir_ent_ptr->last_modified_time));
    putchar(' ');
    print_dir_fat_8_3_date(parse_fat_8_3_date(dir_ent_ptr->last_modified_date));
    putchar('\t');
    printf("%u bytes\n", dir_ent_ptr->size);
}

void print_dir_fat_8_3_time(DIR_FAT_8_3_TIME_t time) {
    printf("%02u:%02u:%02u", time.hour, time.minute, time.second);
}

void print_dir_fat_8_3_date(DIR_FAT_8_3_DATE_t date) {
    // I'm not a yank, live with it
    printf("%02u/%02u/%u", date.day, date.month, date.year);
}

void print_dir_fat_8_3_attrs(uint8_t attrs) {
    if (attrs == LONG_FILE) {
        printf("%s", "LONG_FILE");
        return;
    }
    if (attrs & HIDDEN) {
        printf("%s", "HIDDEN ");
    }
    if (attrs & SYSTEM) {
        printf("%s", "SYSTEM ");
    }
    if (attrs & VOLUME_ID) {
        printf("%s", "VOLUME_ID ");
    }
    if (attrs & DIRECTORY) {
        printf("%s", "DIRECTORY ");
    }
    if (attrs & ARCHIVE) {
        printf("%s", "ARCHIVE ");
    }
}
