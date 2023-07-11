#pragma once
#include <stdint.h>

typedef enum FSType : int32_t {
    FAT12 = 0,
    FAT16 = 1,
    FAT32 = 2,
    exFAT = 3,
    NTFS  = 4,
    UNKNOWN = -1
} FSType ;

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

typedef struct EBR_FAT12_16 {
    uint8_t drive_number;
    uint8_t nt_flags;
    uint8_t signature;
    uint32_t volume_id;
    char volume_label[11];
    char system_identifier_string[8];
    uint8_t boot_code[448];
    uint16_t boot_partition_signature;
} __attribute__((packed)) EBR_FAT12, EBR_FAT16;

typedef struct EBR_FAT32 {
    uint32_t num_sectors_per_fat;
    uint16_t flags;
    uint16_t fat_version_number;
    uint32_t root_directory_cluster_number;
    uint16_t fsinfo_sector_number;
    uint16_t backup_boot_sector_sector_number;
    uint8_t reserved[12]; // Should be zero after format
    uint8_t drive_number;
    uint8_t nt_flags;
    uint8_t signature;
    uint32_t volume_id;
    char volume_label[11];
    char system_identifier_string[8];
    uint8_t boot_code[420];
    uint16_t boot_partition_signature;
} __attribute__((packed)) EBR_FAT32;

// FAT Boot Record 

typedef struct BS_FAT {
    BPB bpb;
    union EBR {
        EBR_FAT12 fat12;
        EBR_FAT16 fat16;
        EBR_FAT32 fat32;
    } ebr;
} __attribute__((packed)) BS_FAT;

// exFAT Boot Sector 

typedef struct BS_exFAT {
    uint8_t entry_point[3];
    char file_system_name[8];
    uint8_t zero_reserved_bytes[53];
    uint64_t partition_offset;
    uint64_t voulme_length;
    uint32_t fat_offset; 
    uint32_t fat_length; 
    uint32_t cluster_heap_offset; 
    uint32_t cluster_count; 
    uint32_t first_cluster_of_root_directory; 
    uint32_t volume_serial_number; 
    uint16_t file_system_revision; 
    uint16_t volume_flags; 
    uint8_t bytes_per_sector_shift; 
    uint8_t sectors_per_cluster_shift; 
    uint8_t num_file_allocation_tables; 
    uint8_t drive_select; 
    uint8_t percentage_in_use; 
    uint8_t reserved_bytes[7]; 
    uint8_t boot_code[390];
    uint16_t boot_partition_signature;
} __attribute__((packed)) BS_exFAT;

/*
 *  Helper Functions
 */

FSType determine_fs_type(BS_FAT* bs);
uint32_t get_num_sectors_in_logical_volume(BPB* bpb);
