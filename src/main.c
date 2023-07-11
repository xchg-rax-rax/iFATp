#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

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
  
    // Read in BIOS Parameter Block so we can identify the FSType
    BS_FAT boot_sector;
    int bytes_read = 0;
    if ((bytes_read = read(fd, &boot_sector, sizeof(BS_FAT))) == -1) {
        // Add better error handling
        puts("[!] Error reading file.");
        return EXIT_FAILURE;
    }
    if (bytes_read != sizeof(BS_FAT)) {
        // Handle particular error correctly
        puts("[!] Failed to read complete BIOS Parameter Block, is image corrupt or truncated?.");
        return EXIT_FAILURE;
    }
    FSType fs_type = determine_fs_type(&boot_sector);

    switch(fs_type) {
        case FAT12:
            printf("[*] FAT12 file-system detected.\n");
            print_fat_12_metadata(&boot_sector);
            break;
        case FAT16:
            printf("[*] FAT16 file-system detected.\n");
            print_fat_16_metadata(&boot_sector);
            break;
        case FAT32:
            printf("[*] FAT32 file-system detected.\n");
            print_fat_32_metadata(&boot_sector);
            break;
        case exFAT:
            printf("[*] exFAT file-system detected.\n");
            break;
        case NTFS:
            printf("[*] NTFS file-system detected.\n");
            break;
        case UNKNOWN:
        default:
            printf("[!] Unknown file-system format, cannot parse.\n");
            break;
    }
    //print_bios_parameter_block(&bios_parameter_block);
    //print_fat_12_metadata(&boot_sector);
    return EXIT_SUCCESS; 
}
