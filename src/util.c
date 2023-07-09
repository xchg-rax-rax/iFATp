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
    char system_identifier_string[9];
    memcpy(system_identifier_string, ebr->system_identifier_string, 8);
    system_identifier_string[8] = '\0';
    printf("System Identifier String: %s\n", system_identifier_string);
    printf("%s","Boot Code:\n");
    print_hex_dump(ebr->boot_code, sizeof(ebr->boot_code));
    printf("%s","Boot Parturition Signature: ");
    print_hex_dump((uint8_t*)&(ebr->boot_partition_signature), sizeof(ebr->boot_partition_signature));
}

void print_fat_12_metadata(BS_FAT12* bs) {
    print_bios_parameter_block(&(bs->bpb));
    print_extended_boot_record(&(bs->ebr));
};
