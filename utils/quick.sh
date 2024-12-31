#!/bin/zsh

# compile
g++ ./bytearray2bin.cpp -o app

# remove bin file 
rm -f img*.bin
#run
./app

# copy and replace to sdcard
cp -rf /mnt/sda1/DOC/HCMUTE_DOCs/NH_2024_2025_HK_01/ITFA336064/a_gift_for_crush/utils/img*.bin /media/msnp/1F6B-E36F/imgs 

#remove app
rm -f app