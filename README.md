# iFATp
The improved FAT parser. Get exhaustive information on FAT12, FAT16, FAT32, and exFAT disk images.

Currently there are no really high quality FAT file parsers out there.
The best one I've found is `fatcat` which while a very feature rich and useful tool for manipulating FAT images, is missing a number of important fields when it comes to displaying detailed metadata on FAT images. iFATp aims to be a comphrehensive FAT file parser that dispalys all the metadata present in a FAT image in a highly structured way. 

iFATp was created as a tool during the process of OS dev and as a consequence it's output is structured with the needs of an individual manually creating and modifying FAT file systems, byte by byte, in mind.

# To Do

- Finish Header Parsing for:
    - ~FAT12~ [DONE]
    - ~FAT16~ [DONE]
    - ~FAT32~ [DONE]
    - ~exFAT~ [DONE]
    - NTFS [Maybe...]
[At present the goal is to implment the following for FAT12/16 only]
- Add computed statistics for file systems [Computed values useful for locating FS structures are done, aspects related to capacity etc still WIP]
- Add File system parsing:
    - Add ability to cat files
    - List Deleted Files
    - Recover Deleted Files
    - Get size and capacity information
- Add complex arg pasring to allow for user friendly access of features
