# About

The C++ program to convert an JPG image to BIN image (that can be read and displayed in `agiftforcrush`). All image in `imgs` folder will be resized and crop to **hxw=220x172** the export to **\*.bin** (destination output) and **\*.jpg** (preview output).

## Dependencies

This C++ program requires `libjpeg` to read/write JPG image file and `g++` to compile `*.cpp` files.

```bash
    sudo apt update
    sudo apt upgrade
    sudo apt install g++ gcc
    sudo apt install libjpeg-dev
```

## Working Tree

```text
    utils
    ├──── archived                              : contains archived imgs, cpp files, bash script files 
    ├──── imgs                                  : original imgs (input img source)
    │     ├──── *.jgp / *.JPG
    │     └──── ...
    ├──── bins                                  : bin imgs (target output)
    │     ├──── img1.bin 
    │     └──── ...
    ├──── resized_imgs                          : resized imgs (preview output)
    │     ├──── resized_img_1.jpg 
    │     └──── ...
    ├──── img2bin.cpp                           : the C++ program to convert JPG img -> BIN img
    ├──── Makefile                              : 
    └──── ...
```

## Useage

### Makefile

First, change the path to SDCard, the simple way, open SDCard dir, then ```ctrl+l```, then copy them. The example below.

```Makefile
[line 25] SDCARD_TARGET   := /media/ngxxfus/SD_CARD
```
![output](https://github.com/user-attachments/assets/4b238669-9ef3-4bf9-a3cc-ccd8b178b3b8)

Seccond, copy all images (which you want to be converted) into ```imgs``` directory.

Third, run with ```make```. Make sure ```make``` has been installed in yours. If ```make``` hasn't been installed, install via ```apt```.

```bash
    sudo apt install make
```

|Make commands          | Decription    |
| :--                   | :--           |
| make / make all       | build object file ```img2bin.o``` and excutable file ```img2bin``` |
| make clean            | remove  ```img2bin.o```, ```img2bin```, ```utils/resized_imgs/*```, ```utils/bins/*```|
| make rebuild          | remove  ```img2bin.o```, ```img2bin```, ```utils/resized_imgs/*```, ```utils/bins/*``` then re-build ```img2bin.o``` and ```img2bin```|
| make convert_images   | if doesn't exist ```img2bin.o```, ```img2bin```, then build ```img2bin.o```, ```img2bin```, then run ```img2bin.o``` to convert images in ```utils/resized_imgs/``` to binary file image ```utils/bins/*``` and preview image ```utils/resized_imgs/``` (includes resize, crop).|
| make copy2sdcard      | Copy binary file image into ```imgs``` directory in SDCard. |

A recommended make command to run
```bash
    make convert_images copy2sdcard
```

