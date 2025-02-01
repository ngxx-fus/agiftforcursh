cpp_file_path:=img2bin.cpp
full_size_img_dir:=none
output_img_dir:=none

all: run 

run: compile clear_output
	# ./img2bin	./imgs/img*.jpg
	./img2bin	./imgs/*.jpg

memcheck: compile
	valgrind --tool=memcheck ./img2bin

compile:
	g++ $(cpp_file_path) -o img2bin -ljpeg -Wall

clear_output:
	rm -rf ./resized_imgs/*

install_dep:
	sudo apt update
	sudo apt upgrade
	sudo apt install g++ gcc
	sudo apt install libjpeg-dev
