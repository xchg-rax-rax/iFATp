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
        printf("[!] Error opening file.");
        return EXIT_FAILURE;
    }
  
    // To start with we will just work on parsing FAT12 (as this is the one which is 
    // of immediate interest to me) but we will have to modify how we are reading the
    // file system down the line, pulling out the BPB first and using it to determine
    // the FAT type. 
    //
    // Here is pseudo code to do that
    //
    // if (cluster_count < 4086) {
    //      // FAT12
    // }
    // else if (cluster_count < 65525) {
    //      // FAT16
    // }
    // else {
    //      // FAT32
    // }
    //
    // For exFAT we should load the BPB as a FAT BPB and then check to see if the bytes 
    // per sector is 0, if it is we're working with exFAT more than likely

    BS_FAT12 boot_sector;

    int bytes_read = 0;
    if ((bytes_read = read(fd, &boot_sector, sizeof(BS_FAT12))) == -1) {
        // Add better error handling
        printf("[!] Error reading file.");
        return EXIT_FAILURE;
    }
    /*
    if (bytes_read != 512) {
        // Handle particular error correctly
        printf("[!] Failed to read complete Boot Sector, is image corrupt/incomplete?.");
        return EXIT_FAILURE;
    }
    */
    print_fat_12_metadata(&boot_sector);
    return EXIT_SUCCESS; 
}
