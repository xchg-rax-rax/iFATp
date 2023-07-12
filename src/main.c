#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/mman.h>

#include "util.h"
#include "fat.h"


int main(int argc, char** argv) {
    if (argc != 2) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        // Add better error handling
        puts("[!] Error opening file.");
        return EXIT_FAILURE;
    }
    
    struct stat statbuf;
    if (fstat(fd, &statbuf) != 0)  {
        // Add better error handling
        puts("[!] Error obtaining file information.");
        return EXIT_FAILURE;
    }

    uint8_t* file_map;
    if ((file_map = mmap(NULL, statbuf.st_size, PROT_READ, MAP_PRIVATE, fd, 0)) == MAP_FAILED) {
        // Add better error handling
        puts("[!] Error mapping file into address space.");
        return EXIT_FAILURE;
    }
  
    BS_FAT* boot_sector = (BS_FAT*) file_map;

    BPBComputedValues_t computed_values = compute_values(boot_sector);

    switch(computed_values.fs_type) {
        case FAT12:
            printf("[*] FAT12 file-system detected.\n");
            print_fat_12_metadata(boot_sector);
            print_computed_values(&computed_values);
            printf("%s","\nRoot Filesystem Entry\n");
            printf("%s","---------------------\n\n");
            DIR_FAT_8_3_t* root_dir_ent = get_dir_fat_8_3(get_root_directory_ptr(file_map));
            printf("Directory Entry Filesystem Offset: %p\n", (void*)((uint8_t*)root_dir_ent-file_map));
            print_dir_fat_8_3(get_dir_fat_8_3(&root_dir_ent[0]));
            print_dir_fat_8_3(get_dir_fat_8_3(&root_dir_ent[1]));
            print_dir_fat_8_3(get_dir_fat_8_3(&root_dir_ent[3]));
            print_dir_fat_8_3(get_dir_fat_8_3(&root_dir_ent[5]));
            break;
        case FAT16:
            printf("[*] FAT16 file-system detected.\n");
            print_fat_16_metadata(boot_sector);
            print_computed_values(&computed_values);
            break;
        case FAT32:
            printf("[*] FAT32 file-system detected.\n");
            print_fat_32_metadata(boot_sector);
            print_computed_values(&computed_values);
            break;
        case exFAT:
            printf("[*] exFAT file-system detected.\n");
            print_exfat_metadata((BS_exFAT*)boot_sector);
            break;
        case NTFS:
            printf("[*] NTFS file-system detected.\n");
            break;
        case UNKNOWN:
        default:
            printf("[!] Unknown file-system format, cannot parse.\n");
            break;
    }
    return EXIT_SUCCESS; 
}
