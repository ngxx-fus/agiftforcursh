#ifndef CONFIG_UTILS_H
#define CONFIG_UTILS_H

#if SHOW_AUTHOR_MESSAGE == true
    #pragma message("\nIncluded config_utils.h!\n")
#endif

/// >>>>>>>>>>>>>>>>>>>>> HEADER INCLUDES >>>>>>>>>>>>>>>>>>>>>>>>>>
#include "Arduino.h"
#include "serial_utils.h"
#include "sdcard_utils.h"


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


namespace local_config{
    /// >>>>>>>>>>>>>>>>>>>>> GLOBAL DEFS >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

    #define __bytes
    #define CONFIG_FILE_PATH        "/config/conf.bin"

    /// >>>>>>>>>>>>>>>>>>>>> GLOBAL VARS >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

    static const uint8_t PROGMEM    max_size            = 64 __bytes;
    static const uint8_t PROGMEM    segment_size        = 32 __bytes;
    uint8_t*                        config_file         = nullptr;
    bool                            is_opened           = false;
    
    /// >>>>>>>>>>>>>>>>>>>>> OTHER DEFINITIONS >>>>>>>>>>>>>>>>>>>>>>>>
    bool open_config_file(){
        #if LOG == true
        call("import_config_file");
        #endif
        if(esp_get_free_heap_size() < 64 __bytes){
            #if LOG == true
                log2ser("esp_get_free_heap_size: ", esp_get_free_heap_size());
                log2ser("Not enough heap space to store the config!");
            #endif
            is_opened = false;
            return false;
        }

        /// open config file in SDcard
        File file = SD.open(CONFIG_FILE_PATH);
        if (!file){
            is_opened = false;
            #if LOG == true
                log2ser( "Failed to open `config_file`!");
            #endif
            return false;
        }
        if(file.isDirectory() == true){
            #if LOG == true
                log2ser( "Err: isDirectory!");
            #endif
            is_opened = false;
            #if LOG == true
                log2ser( "Failed to open `config_file`!");
            #endif
            return false;
        }

        /// Allocate 64 bytes for the config
        config_file = new uint8_t[local_config::max_size];

        auto read_size = file.read(config_file, max_size);

        #if LOG == true
            log2ser("read ", read_size, "/",max_size, " `config_file`!");
        #endif

        if( read_size < max_size){
            #if LOG == true
                log2ser("Err: `config_file` may be corrupted!");
            #endif
            delete[] config_file;
            config_file = nullptr;
            is_opened = false;
            #if LOG == true
                log2ser( "Failed to open `config_file`!");
            #endif
            return false;
        };

        #if LOG == true
            log2ser("`config_file` is openned at ", String((int)config_file, HEX), "!");
        #endif
        is_opened = true;
        return true;
    }

    void close_config_file(){
        #if LOG == true
            call("close_config_file");
        #endif
        is_opened = false;
        delete[] config_file;
        config_file = nullptr;
        #if LOG == true
            log2ser("`config_file` is closed!");
        #endif
    }

    /// @brief allocate `max_size` bytes to store the config
    /// if non-existed `config_file`
    uint8_t* generate_config_file(){
        #if LOG == true
            call("generate_config_file");
        #endif
        if( config_file != nullptr ){
            delete[] config_file;
        }
        if(esp_get_free_heap_size() < 64 __bytes){
            #if LOG == true
                log2ser("esp_get_free_heap_size: ", esp_get_free_heap_size());
                log2ser("Not enough heap space to store the config!");
            #endif
            is_opened = false;
            return nullptr;
        }

        is_opened = true;
        
        config_file = new uint8_t[max_size];
        return config_file;
    }

    bool save_config_file(){

        return true;
    }

    /// @brief Get a byte in config file
    /// @param segment_id   index of segment
    /// @param byte_id      index of byte in the segment
    /// @return the byte corresponding to  `segment_id` and `byte_id`,
    /// if `segment_id` and `byte_id` is invalid, return 0xFF.
    uint8_t get_config_byte(uint8_t segment_id, uint8_t byte_id){
        if(!is_opened){
            #if LOG == true
                log2ser("Err: No config file is opened!");
            #endif
            return 0xFF;
        }
        if(segment_id * segment_size + byte_id >= max_size){
            #if LOG == true
                log2ser("Err: Invalid querry! (index not found)");
            #endif
            return 0xFF;
        }
        return *(config_file + segment_id * segment_size + byte_id);
    }

    /// @brief Set a byte in config file
    /// @param segment_id   index of segment
    /// @param byte_id      index of byte in the segment
    /// @param byte_val     the value will be set
    /// @return 0 if success!, otherwise 0xFF.
    uint8_t set_config_byte(uint8_t segment_id, uint8_t byte_id, uint8_t byte_val){
        if(!is_opened){
            #if LOG == true
                log2ser("Err: No config file is opened!");
            #endif
            return 0xFF;
        }
        if(segment_id * segment_size + byte_id >= max_size){
            #if LOG == true
                log2ser("Err: Invalid querry! (index not found)");
            #endif
            return 0xFF;
        }
        *(config_file + segment_id * segment_size + byte_id) = byte_val;
        return 0;
    }

};

void config_init(){
    #if LOG == true
    call("");
    #endif
}

#endif