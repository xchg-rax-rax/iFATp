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

// Correct name to be BS_FAT
typedef struct BPBComputedValues {
    uint32_t total_sectors;
    uint32_t num_sectors_per_fat;
    uint32_t num_root_dir_sectors;
    uint32_t first_fat_sector_index;
    uint32_t first_data_sector_index;
    uint32_t num_data_sectors;
    uint32_t num_clusters;
    FSType   fs_type;
} BPBComputedValues_t;

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
 *  Directory Structs
 */
//perhaps these should just be consts?
typedef enum DIR_ATTR {
    READ_ONLY=0x01,
    HIDDEN=0x02,
    SYSTEM=0x04,
    VOLUME_ID=0x08,
    DIRECTORY=0x10,
    ARCHIVE=0x20
} DIR_ATTR_t;

typedef struct DIR_FAT_8_3 {
    char file_name[8];
    uint8_t attributes;
    uint8_t nt_reserved;
    uint8_t creation_time_tenths_sec;
    uint16_t creation_time;
    uint16_t creation_date;
    uint16_t last_access_date;
    uint16_t first_cluster_number_upper;
    uint16_t last_modified_time;
    uint16_t last_modified_date;
    uint16_t first_cluster_number_lower;
    uint32_t size;
} __attribute__((packed)) DIR_FAT_8_3_t;

typedef struct DIR_FAT_LONG_FILENAME {
    uint8_t index;
    uint16_t first_utf16_chars[5];
    uint8_t lf_attribute; // check to verify this a lf entry
    uint8_t lf_entry_type;
    uint8_t checksum;
    uint16_t second_utf16_chars[6];
    uint16_t zero_reserved;
    uint16_t final_utf16_chars[2];
} __attribute__((packed)) DIR_FAT_LONG_FILENAME_t;

/*
 *  Helper Functions
 */

void determine_fs_type(BPBComputedValues_t* computed_values, BS_FAT* bs);
uint32_t get_num_sectors_in_logical_volume(BPB* bpb);
// can probably do without the explicit bpb ptr here
uint16_t get_next_cluster_fat12(BPB* bpb, uint8_t* file_system, uint16_t active_cluster);
uint8_t* get_root_directory_ptr(uint8_t* file_system);
BPBComputedValues_t compute_values(BS_FAT* bs);
