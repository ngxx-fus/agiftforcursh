#!/bin/zsh

###################### text effects ######################
BOLD="\e[1m"
GREEN="\e[32m"
YELLOW="\e[33m"
BLUE="\e[34m"
NORMAL="\e[0m"

###################### main script ######################

printf "\n${BOLD}${YELLOW}INFO${NORMAL}: clear screen\n"
clear

printf "\n${BOLD}${YELLOW}INFO${NORMAL}:
Hi
In my firmware that i've flashed into ESP32, i've set \
the name of images is formed img\*.bin and stored at \
/imgs/, '/' is root of the SD card.
"

printf "\n${BOLD}${YELLOW}INFO${NORMAL}: compiling img2bin.cpp\n"
g++ img2bin.cpp -o app -ljpeg

printf "\n${BOLD}${YELLOW}INFO${NORMAL}: removing img*.bin files \n"
rm -f ./bins/img*.bin
rm -f /media/msnp/1F6B-E36F/imgs/img*.bin

printf "\n${BOLD}${YELLOW}INFO${NORMAL}: running ./app .jpg .bin\n"
./app ./imgs/img0.jpg  ./bins/img0.bin
./app  ./imgs/img1.jpg  ./bins/img1.bin
./app  ./imgs/img2.jpg  ./bins/img2.bin
./app  ./imgs/img3.jpg  ./bins/img3.bin
./app  ./imgs/img4.jpg  ./bins/img4.bin
./app  ./imgs/img5.jpg  ./bins/img5.bin

printf "\n${BOLD}${YELLOW}INFO${NORMAL}: copying and replacing to sdcard\n"
cp -rf ./bins/img*.bin /media/ngxxfus/4302-ECDD/imgs
