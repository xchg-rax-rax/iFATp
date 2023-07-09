#pragma once
#include "fat.h"

// maybe make args const const*

void print_usage(char* bin);

void print_bios_parameter_block(BPB* bpb);

void print_fat_12_metadata(BS_FAT12* bs);

//void print_fat_16_metadat();

