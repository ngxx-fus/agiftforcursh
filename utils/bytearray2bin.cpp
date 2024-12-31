#include <bits/stdc++.h>
using namespace std;

#include "images.h"

#define bytes_array img
#define arr_size 172*220
#define bin_filename "img0.bin"

void writebytes(string filename, const uint16_t* img){
    std::ofstream fs(filename, std::ios::out | std::ios::binary | std::ios::app);
    for(int i = 0; i < arr_size; ++i){
        const static char byte_h = ((img[i])&0xFF00)>>8;
        fs.ostream::write(&byte_h, 1);
        const char byte_l = (img[i])&0x00FF;
        fs.ostream::write(&byte_l, 1);
    }
    fs.close();
}

int main(){
    writebytes("img0.bin", imgallArray[0]);
    writebytes("img1.bin", imgallArray[1]);
    writebytes("img2.bin", imgallArray[2]);
    writebytes("img3.bin", imgallArray[3]);
}