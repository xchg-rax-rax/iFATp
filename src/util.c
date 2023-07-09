#include <stdio.h>
#include <string.h>

#include "util.h"


void print_usage(char* bin) {
    printf("%s <path_to_disk_image>\n", bin);
}

void print_bios_parameter_block(BPB* bpb) {
    printf("Entry Point: %X %X %X\n", 
            bpb->entry_point[0], 
            bpb->entry_point[1], 
            bpb->entry_point[2]);

    // string is not null terminated so we need to make a copy that is
    char oem_identifier[9];
    memcpy(oem_identifier, bpb->oem_identifier, 8);
    oem_identifier[8] = '\0';
    printf("OEM Identifier: %s\n", oem_identifier);

    printf("Bytes Per Sector: %u\n", bpb->num_bytes_per_sector);

    uint16_t num_bytes_per_sector = bpb->num_bytes_per_sector != 0 ? 
        bpb->num_bytes_per_sector : bpb->large_sector_count;
    printf("Sectors Per Cluster: %u\n", num_bytes_per_sector);

    printf("Number of Reserved Sectors: %u\n", bpb->num_root_directory_entires);
    printf("Number of File Allocation Tables: %u\n", bpb->num_file_allocation_tables);
    printf("Number of Root Directory Entries: %u\n", bpb->num_root_directory_entires);
    printf("Sectors Per Logical Volume: %u\n", bpb->num_sectors_in_logical_volume);
    printf("Media Description Type: %X\n", bpb->media_description_type);
    printf("Sectors Per File Allocation Tables: %u\n", bpb->num_sectors_per_fat);
    printf("Sectors Per Track: %u\n", bpb->num_sectors_per_track);
    printf("Number of Heads: %u\n", bpb->num_heads);
    printf("Number of Hidden Sectors: %u\n", bpb->num_hidden_sectors);
}

void print_extended_boot_record(EBR_FAT12* ebr) {
    printf("Drive Number: %u\n", ebr->drive_number);
    printf("NT Flags: %X\n", ebr->nt_flags);
    printf("Signature: %X\n", ebr->signature);
    printf("Volume ID: %X\n", ebr->volume_id);
    char volume_label[12];
    memcpy(volume_label, ebr->volume_label, 11);
    volume_label[11] = '\0';
    printf("Volume Label: %s\n", volume_label);
}

void print_fat_12_metadata(BS_FAT12* bs) {
    print_bios_parameter_block(&(bs->bpb));
    print_extended_boot_record(&(bs->ebr));
};
