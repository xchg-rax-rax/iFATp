# iFATp
The improved FAT parser. Get exhaustive information on FAT12, FAT16, FAT32, and exFAT disk images.

Currently there are no really high quality FAT file parsers out there.
The best one I've found is `fatcat` which while a very feature rich and useful tool for manipulating FAT images, is missing a number of important fields when it comes to displaying detailed metadata on FAT images. iFATp aims to be a comphrehensive FAT file parser that dispalys all the metadata present in a FAT image in a highly structured way. 

iFATp was created as a tool during the process of OS dev and as a consequence it's output is structured with the needs of an individual manually creating and modifying FAT file systems, byte by byte, in mind.

# To Do

- Add code to recognize FS types
- Finish Header Parsing for:
    - ~FAT12~ [DONE]
    - ~FAT16~ [DONE]
    - ~FAT32~ [DONE]
    - ~exFAT~ [DONE]
    - NTFS [Maybe...]
- Add computed statistics for file systems
- Add File system parsing:
    - List Files and directories 
    - List Deleted Files
    - Recover Deleted Files
    - Get size and capacity information
