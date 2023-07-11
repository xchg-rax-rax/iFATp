#include "fat.h"

#include <stdio.h>

uint64_t compute_count_of_sectors(BS_FAT* bs) {
    uint64_t root_dir_sectors = ((bs->bpb.num_root_directory_entires*32) + (bs->bpb.num_bytes_per_sector-1)) / bs->bpb.num_bytes_per_sector;
    uint32_t num_sectors_per_fat;
    if(bs->bpb.num_sectors_per_fat != 0) {
        num_sectors_per_fat = bs->bpb.num_sectors_per_fat;
    }
    else {
        num_sectors_per_fat = bs->ebr.fat32.num_sectors_per_fat;
    }
    uint32_t total_sectors;
    if(bs->bpb.num_sectors_in_logical_volume != 0) {
        total_sectors = bs->bpb.num_sectors_in_logical_volume;
    }
    else {
        total_sectors = bs->bpb.large_sector_count;
    }
    uint64_t num_data_sectors = total_sectors - (bs->bpb.num_reserved_sectors + (bs->bpb.num_file_allocation_tables*num_sectors_per_fat) + root_dir_sectors);
    return num_data_sectors / bs->bpb.num_sectors_per_cluster;
}

    

FSType determine_fs_type(BS_FAT* bs) {
    if(bs->bpb.num_bytes_per_sector == 0) {
        return exFAT;
    }
    uint64_t cluster_count = compute_count_of_sectors(bs);
    if (cluster_count < 4086) {
        return FAT12; 
    }
    else if (cluster_count < 65525) {
        return FAT16; 
    }
    else if(bs->bpb.num_root_directory_entires == 0) {
        return FAT32; 
    }
    else {
        return UNKNOWN;
    }
}

inline uint32_t get_num_sectors_in_logical_volume(BPB* bpb) {
    if (bpb->num_sectors_in_logical_volume == 0) {
        return bpb->large_sector_count;
    }
    return bpb->num_sectors_in_logical_volume;
}
