#ifndef SDCARD_UTILS_H
#define SDCARD_UTILS_H

#if SHOW_AUTHOR_MESSAGE == true
    #pragma message("\nIncluded sdcard_utils.h!\n")
#endif


#ifndef rep
#define rep(i,a,b) for(int i = (a); i <= (b); ++i)
#endif
#ifndef rev
#define rev(i,a,b) for(int i = (b); i >= (a); --i)
#endif
#ifndef revt
#define rept(type, i,a,b) for(type i = (a); i <= (b); ++i)
#endif
#ifndef revt
#define revt(type, i,b, a) for(type i = (b); i >= (a); --i)
#endif

#include <vector>
#include <queue>

#include "general_utils.h"

#include "FS.h"
#include "SD.h"
#include "SPI.h"

using namespace std;

#ifndef SDCARD_SPI_CS_PIN
    #define SDCARD_SPI_CS_PIN 5
#endif

/// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
/// >>>>>                                                      >>>>>
/// >>>>>                                                      >>>>>
/// >>>>>                                                      >>>>>
/// >>>>>                                                      >>>>>
/// >>>>>>>>>>>>>>>>>>>>> NAMESPACE SDCCARD_IMGS >>>>>>>>>>>>>>>>>>>

namespace sdcard_imgs{

    bool            is_available                = true;
    String          imgs_path                   = "/imgs";
    vector<String>  img_list;
    String          filename_get_extension(String filename, uint16_t mode);
    uint32_t        list();
    bool            sdcard_load_565img(uint16_t img[172][220], String filename);
    bool            cache(uint32_t img_index);
    /// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> ///

    String filename_get_extension(String filename, uint16_t mode){
        /// mode = 0x1 : to_lowercase
        /// mode = 0x2 : to_uppercase
        String res = "";
        uint8_t dot = 0;
        if(filename.isEmpty()) return res;
        if(
            filename.charAt(filename.length()-1) == '/'||
            filename.charAt(filename.length()-1) == '\\'
        )   filename.remove(filename.length()-1);
        revt(uint8_t, i, filename.length()-1, 0)
            if(filename.charAt(i) != '.'){
                res += filename.charAt(i);
            }else{
                break;
            }

        reverse(res.begin(), res.end());
        
        switch (mode){
            case 0x1: res.toLowerCase(); break;
            case 0x2: res.toUpperCase(); break;
        }

        return res;
    }

    void sdcard_ls(vector<String>& list, String path = "/", uint16_t level = 100)
    {
        #if LOG == true
            call("sdcard_imgs::", "sdcard_ls");
        #endif
        File root = SD.open(path);
        if (!root) {
            #if LOG == true
                log2ser("sdcard_imgs::", "sdcard_ls", "cannot open ", path);
            #endif
            sdcard_imgs::is_available = false;
            return;
        }

        sdcard_imgs::is_available = true;
        File file = root.openNextFile();

        while (file){
            if (file.isDirectory()){
                if(level){
                    sdcard_ls(list, file.name(), level - 1);
                }else{
                    continue;
                }
            }else{
                String lowercase_extension = filename_get_extension(file.name(), 0x1);
                if(lowercase_extension == "bin"){
                    list.push_back(concatenate(path, "/", file.name()));
                }
            }
            file = root.openNextFile();
        }
        file.close();
    }

    uint32_t list(){
        #if LOG == true
            call("sdcard_imgs::list");
        #endif
        sdcard_ls(img_list, imgs_path);
        return img_list.size();
    }

    bool cache_and_insert(CANVAS<uint16_t> &canvas, uint32_t img_index){
        #if LOG == true
            call("sdcard_imgs::cache_and_insert");
        #endif
        #if SDCARD_RW == true
            if(sdcard_imgs::is_available == false){
                #if LOG == true
                    log2ser("sdcard_imgs::is_available: ", sdcard_imgs::is_available?'Y':'N');
                #endif
                canvas.refill(0xFFFF);
                single_text_line(1, "[SDCard]");
                single_text_line(2, "un-available!");
                return false;
            }
            if( img_index >= img_list.size()){
                #if LOG == true
                    log2ser("sdcard_imgs::cache_and_insert: ", "out of range");
                #endif
                canvas.refill(0xFFFF);
                single_text_line(1, "[Images list]");
                single_text_line(2, "out of range!");
                return false;
            };

            File file = SD.open(img_list[img_index], FILE_READ);
            
            #if LOG == true
                log2ser("sdcard_imgs::cache_and_insert: open ", img_list[img_index]);
            #endif

            if(!file){
                #if LOG == true
                    log2ser("sdcard_imgs::cache_and_insert: failed to open ", img_list[img_index]);
                #endif
                sdcard_imgs::is_available = false;
                canvas.refill(0xFFFF);
                single_text_line(1, "[Images]");
                single_text_line(2, "failed to open:");
                single_text_line(3, img_list[img_index], 0x0, 5, 23U, true);
                return false;
            }
            uint8_t  buf16[2];
            uint16_t r, c, _565pixel = 0xf802;
            rept(uint16_t, i, 0, 220*172-1){
                r = uint16_t(i/172), c = uint16_t(i%172); 
                file.read(buf16, 2);
                _565pixel = ((uint16_t(buf16[0])&0xFF) << 8) | (uint16_t(buf16[1])&0xFF);
                canvas.set_pixel({r, c}, _565pixel);
            }
            sdcard_imgs::is_available = true;
            file.close();
            return true;
        #else
            canvas.refill(0xFFFF);
            sdcard_imgs::is_available = false;
            return false;
        #endif
    }

};



/// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
/// >>>>>                                                      >>>>>
/// >>>>>                                                      >>>>>
/// >>>>>                                                      >>>>>
/// >>>>>                                                      >>>>>
/// >>>>>>>>>>>>>>>>>>>>> OTHER DEFINATION >>>>>>>>>>>>>>>>>>>>>>>>>

void sdcard_init(){
    #if LOG == true
        call("sdcard_init");
    #endif

    SD.end();

    if(!SD.begin(SDCARD_SPI_CS_PIN)){
        #if LOG == true
            log2ser( "SDcard: failed to mount");
        #endif
        sdcard_imgs::is_available = false;
        return;
    }
    if(SD.cardType() == CARD_NONE){
        sdcard_imgs::is_available = false;
        #if LOG == true
            log2ser( "SDcard: No SDcard");
        #endif
        #if BASIC_IO == true
            basic_io::led0_blinky(5, 15, 30);
        #endif
        return;
    }

    #if LOG == true
    log2ser( "SDcard: mounted");
    #endif
    switch(SD.cardType()){
        case CARD_MMC: log2ser( "SDcard: Type: MMC"); break;
        case CARD_SD: log2ser( "SDcard: Type: SDSC"); break;
        case CARD_SDHC: log2ser( "SDcard: Type: SDHC"); break;
        default: log2ser( "SDcard: Type: ?");
    }
    sdcard_imgs::is_available = true;
}

#endif