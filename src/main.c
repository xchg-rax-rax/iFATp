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
            size_t dir_ent_count = 0;
            char path[] = "/TEST_DIR/C";
            DIR_FAT_8_3_t** dir_entries = list_directory_contents(file_map, &dir_ent_count, path);
            for (int i = 0; i < dir_ent_count; i++) {
                print_dir_fat_8_3_short(dir_entries[i]);
            }
            free(dir_entries);
            
            /*
            printf("%s","\nRoot Filesystem Entry\n");
            printf("%s","---------------------\n\n");
            size_t root_dir_ent_count;
            DIR_FAT_8_3_t** root_dir_entries = parse_root_directory(file_map, &root_dir_ent_count);
            printf("Root Directory Entry Count: %lu\n\n", root_dir_ent_count);
            for (int i = 0; i < root_dir_ent_count; i++) {
                print_dir_fat_8_3_short(root_dir_entries[i]);
                if ((root_dir_entries[i])->attributes & DIRECTORY) {
                    uint32_t dir_ent_count = 0;
                    DIR_FAT_8_3_t** dir_entries = parse_directory(file_map, root_dir_entries[i]->first_cluster_number_lower, &dir_ent_count);
                    printf("\n\tDirectory Entries: %d\n\n", dir_ent_count);
                    for (int j = 0; j < dir_ent_count; j++) {
                        putchar('\t');
                        print_dir_fat_8_3_short(dir_entries[j]);
                    }
                    putchar('\n');
                }
            }
            free(root_dir_entries);
            */
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
