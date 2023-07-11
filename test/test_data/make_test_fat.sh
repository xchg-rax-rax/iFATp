#!/bin/bash

dd if=/dev/zero of=fat12.img bs=512 count=2880
mkfs.fat -F 12 -n "Fatima" fat12.img
dd if=/dev/zero of=fat16.img bs=512 count=64000
mkfs.fat -F 16 -n "FAT16img" fat16.img
dd if=/dev/zero of=fat32.img bs=512 count=128000
mkfs.fat -F 32 -n "FAT32img" fat32.img
