#pragma once
#include "fat.h"

// maybe make args const const*

void print_usage(char* bin);

//void print_bios_parameter_block(BPB* bpb);

void print_fat_12_metadata(BS_FAT* bs);
void print_fat_16_metadata(BS_FAT* bs);
void print_fat_32_metadata(BS_FAT* bs);
void print_exfat_metadata(BS_exFAT* bs);
void print_computed_values(BPBComputedValues_t* computed_values);
void print_fs_type(FSType fs_type);
void print_dir_fat_8_3(DIR_FAT_8_3_t* dir_ent_ptr);

