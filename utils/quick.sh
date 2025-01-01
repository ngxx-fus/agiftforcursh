#!/bin/zsh

printf "\nclear screen\n"
clear

printf "\ncompile\n"
g++ img2bin.cpp -o app -ljpeg

printf "\nremove bin file \n"
rm -f /mnt/sda1/DOC/HCMUTE_DOCs/NH_2024_2025_HK_01/ITFA336064/a_gift_for_crush/ ./bins/img*.bin
rm -f /media/msnp/1F6B-E36F/imgs/img*

printf "\nrun\n"
./app ./imgs/img0.jpg  ./bins/img0.bin
./app  ./imgs/img1.jpg  ./bins/img1.bin
./app  ./imgs/img2.jpg  ./bins/img2.bin
./app  ./imgs/img3.jpg  ./bins/img3.bin
./app  ./imgs/img4.jpg  ./bins/img4.bin
./app  ./imgs/img5.jpg  ./bins/img5.bin

printf "\ncopy and replace to sdcard\n"
cp -rf ./bins/img*.bin /media/msnp/1F6B-E36F/imgs/ 
