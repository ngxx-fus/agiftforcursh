#ifndef CONFIG_UTILS_H
#define CONFIG_UTILS_H

/// >>>>>>>>>>>>>>>>>>>>> HEADER INCLUDES >>>>>>>>>>>>>>>>>>>>>>>>>>
#include "Arduino.h"
#include "serial_utils.h"
#include "sdcard_utils.h"
/// just a unit :> for more clearly
#define BYTES 

/*
segment0:
    ├────   byte 0:                         screen mode
    ├────   byte 1:
            ├────   bit0:                   slideshow mode / show env info
            ├────   bit1:                   slideshow mode / light on
            ├────   bit2:                   reserved
            ├────   bit3:                   reserved
            ├────   bit4:                   reserved
            ├────   bit5:                   reserved
            └────   bit7:                   reserved
    ├────   byte [2...3]:                   slideshow mode / image duration
    ├────   byte [4...5]:                   slideshow mode / env sensors refresh interval
    ├────   byte 6:
    ├────   byte 7:
    ├────   byte 8:
    ├────   byte 9:
    ├────   byte10:
    └────   byte31:

*/

/// >>>>>>>>>>>>>>>>>>>>> GLOBAL VARS >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

static const uint8_t PROGMEM max_config_size = 64 BYTES;

/// >>>>>>>>>>>>>>>>>>>>> OTHER DEFINITIONS >>>>>>>>>>>>>>>>>>>>>>>>

bool import_config_file(){
    #if LOG == true
    call("import_config_file");
    #endif
    if(esp_get_free_heap_size() < 64 BYTES){
        #if LOG == true
            log2ser("esp_get_free_heap_size: ", esp_get_free_heap_size());
            log2ser("Not enough heap space to store the config!");
        #endif
        return false;
    }
    /// Allocate 64 bytes for the config
    uint8_t* config_file = new uint8_t[max_config_size];

    return true;
}

void export_config_file(){
    #if LOG == true
    call("export_config_file");
    #endif
}

void slideshow_import_config(){
    #if LOG == true
    call("slideshow_import_config");
    #endif
}

void config_init(){
    #if LOG == true
    call("");
    #endif
}

#endif