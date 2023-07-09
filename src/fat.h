#pragma once
#include <stdint.h>


// BIOS Parameter Block
typedef struct BPB {
    uint8_t entry_point[3];
    char oem_identifier[8];
    uint16_t num_bytes_per_sector;
    uint8_t num_sectors_per_cluster;
    uint16_t num_reserved_sectors;
    uint8_t num_file_allocation_tables;
    uint16_t num_root_directory_entires;
    uint16_t num_sectors_in_logical_volume;
    uint8_t media_description_type;
    uint16_t num_sectors_per_fat; // FAT12/16 only
    uint16_t num_sectors_per_track;
    uint16_t num_heads;
    uint32_t num_hidden_sectors;
    uint32_t large_sector_count; // Only set if num_sectors_per_cluster == 0
 } __attribute__((packed)) BPB;

// FAT12 / FAT16 Extended Boot Record

typedef struct EBR_FAT12 {
    uint8_t drive_number;
    uint8_t nt_flags;
    uint8_t signature;
    uint32_t volume_id;
    char volume_label[11];
    char system_identifier_string[8];
    uint8_t boot_code[448];
    uint16_t boot_partition_signature;
} __attribute__((packed)) EBR_FAT12;


typedef EBR_FAT12 EBR_FAT16;

// FAT12 Boot Record 

typedef struct BS_FAT12 {
    BPB bpb;
    EBR_FAT12 ebr;
} __attribute__((packed)) BS_FAT12;

/*
// FAT16 Boot Record

struct BS_FAT16 {
    BPB bpb;
    EBR_FAT16 ebr;
} __attribute__((packed)) BS_FAT16;
*/
