cpp_file_path:=img2bin.cpp

all: run

run: compile 
	./img2bin	./imgs/*

memcheck: compile
	valgrind --tool=memcheck ./img2bin

compile:
	g++ $(cpp_file_path) -o img2bin -ljpeg -Wall

clear_output:
	rm -rf ./resized_imgs/*
	rm -rf ./bins/*

coppy_to_sdcard:
	mkdir -p /media/ngxxfus/SD_CARD/imgs
	cp -rf ./bins/*.bin  /media/ngxxfus/SD_CARD/imgs

install_dep:
	sudo apt update
	sudo apt upgrade
	sudo apt install g++ gcc
	sudo apt install libjpeg-dev

