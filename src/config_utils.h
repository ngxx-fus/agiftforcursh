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
    ├────   byte [6...7]:                   slideshow mode / env / x_pos
    ├────   byte [8...9]:                   slideshow mode / env / y_pos
    ├────   byte 10:                        light_level
    ├────   byte 11:                        image_inndex
    ├────   byte 12:
    ├────   ...
    └────   byte31:

*/


namespace local_config{
    /// >>>>>>>>>>>>>>>>>>>>> GLOBAL DEFS >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

    #define __bytes
    #define CONFIG_FILE_PATH        "/config/conf.bin"

    /// >>>>>>>>>>>>>>>>>>>>> GLOBAL VARS >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

    static const uint8_t PROGMEM    max_size            = 64 __bytes;
    static const uint8_t PROGMEM    segment_size        = 32 __bytes;
    uint8_t*                        config         = nullptr;
    bool                            is_cached           = false;
    
    /// >>>>>>>>>>>>>>>>>>>>> OTHER DEFINITIONS >>>>>>>>>>>>>>>>>>>>>>>>
    
    
    bool cache_config(){
        #if LOG == true
            call("cache_config");
        #endif

        if(sdcard_imgs::is_available == false){
            sdcard_init();
            if(sdcard_imgs::is_available == false){
                #if LOG == true
                    log2ser("sdcard_imgs::is_available: ", sdcard_imgs::is_available?'Y':'N');
                #endif
                return false;
            }
        }

        if(esp_get_free_heap_size() < max_size __bytes){
            #if LOG == true
                log2ser("esp_get_free_heap_size: ", esp_get_free_heap_size());
                log2ser("Not enough heap space to store the config!");
            #endif
            is_cached = false;
            return false;
        }

        /// open config file in SDcard
        File config_file = SD.open(CONFIG_FILE_PATH);
        if(!config_file){
            #if LOG == true
                log2ser("SD.exists(\"/config\"): ", SD.exists("/config")?'Y':'N');
            #endif
            if (!SD.exists("/config")) SD.mkdir("/config");
            config_file = SD.open(CONFIG_FILE_PATH, "w");
            if(!config_file){
                config_file.close();
                #if LOG == true
                    log2ser("Failed to open ", CONFIG_FILE_PATH);
                #endif
                return false;
            }
        }
        if(config_file.isDirectory() == true){
            #if LOG == true
                log2ser( "Error: isDirectory!");
            #endif
            is_cached = false;
            #if LOG == true
                log2ser( "Failed to open `config`!");
            #endif
            return false;
        }

        /// Allocate 64 bytes for the config
        config = new uint8_t[local_config::max_size];

        auto read_size = config_file.read(config, max_size);

        #if LOG == true
            log2ser("read ", read_size, "/",max_size, " bytes `config`!");
        #endif

        if( read_size < max_size ){
            #if LOG == true
                log2ser("Err: `config` may be corrupted!");
            #endif
            delete[] config;               config = nullptr;
            is_cached = false;
            return false;
        };

        #if LOG == true
            log2ser("`config` is openned at ", String((int)config, HEX), "!");
        #endif
        config_file.close();
        is_cached = true;
        return true;
    }

    void flush_config(){
        #if LOG == true
            call("flush_config");
        #endif
        is_cached = false;
        delete[] config;    config = nullptr;
        #if LOG == true
            log2ser("`config` is flushed!");
        #endif
    }

    /// @brief allocate `max_size` bytes to store the config
    /// if non-existed `config`
    uint8_t* generate_config(){
        #if LOG == true
            call("generate_config");
        #endif

        if(sdcard_imgs::is_available == false){
            sdcard_init();
            if(sdcard_imgs::is_available == false){
                #if LOG == true
                    log2ser("sdcard_imgs::is_available: N");
                #endif
                return nullptr;
            }
        }

        if(is_cached == true || config != nullptr){
            #if LOG == true
                log2ser("`config` existed!");
                log2ser("Delete existed `config`!");
            #endif
            delete[] config;                   config = nullptr;
        }

        if(esp_get_free_heap_size() < max_size __bytes){
            #if LOG == true
                log2ser("esp_get_free_heap_size: ", esp_get_free_heap_size());
                log2ser("Not enough heap space to store the config!");
            #endif
            is_cached = false;
            return nullptr;
        }

        is_cached = true;
        config = new uint8_t[max_size];
        return config;
    }

    bool save_config(){
        #if LOG == true
            call("save_config");
        #endif

        if(is_cached == false){
            #if LOG == true
                log2ser("Failed! No config existed! ");
            #endif
            return false;
        }

        if(sdcard_imgs::is_available == false){
            sdcard_init();
            if(sdcard_imgs::is_available == false){
                #if LOG == true
                    log2ser("sdcard_imgs::is_available: ", sdcard_imgs::is_available?'Y':'N');
                #endif
                return false;
            }
        }

        File config_file = SD.open(CONFIG_FILE_PATH, "w");
        if(!config_file){
            #if LOG == true
                log2ser("SD.exists(\"/config\"): ", SD.exists("/config")?'Y':'N');
            #endif
            if (!SD.exists("/config")) SD.mkdir("/config");
            config_file = SD.open(CONFIG_FILE_PATH, "w");
            if(!config_file){
                config_file.close();
                #if LOG == true
                    log2ser("Failed to open ", CONFIG_FILE_PATH);
                #endif
                return false;
            }
        }

        auto written_bytes = config_file.write(config, max_size __bytes);

        #if LOG == true
            log2ser("Written ",written_bytes, "/", max_size, " bytes `config`!" );
        #endif

        if( written_bytes != max_size __bytes){
            return false;
        }

        return true;
    }

    /// @brief Get a byte in config file
    /// @param segment_id   index of segment
    /// @param byte_id      index of byte in the segment
    /// @return the byte corresponding to  `segment_id` and `byte_id`,
    /// if `segment_id` and `byte_id` is invalid, return 0xFF.
    template<class Treturn = uint8_t>
    Treturn byte_config(uint8_t segment_id, uint8_t byte_id){
        #if LOG == true
            call("byte_config");
        #endif
        if(sdcard_imgs::is_available == false){
            sdcard_init();
            if(sdcard_imgs::is_available == false){
                #if LOG == true
                    log2ser("sdcard_imgs::is_available: N");
                #endif
                return 0xFF;
            }
        }
        if(!is_cached){
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
        return *(config + segment_id * segment_size + byte_id);
    }

    /// @brief Set a byte in config file
    /// @param segment_id   index of segment
    /// @param byte_id      index of byte in the segment
    /// @param byte_val     the value will be set
    /// @return 0 if success!, otherwise 0xFF.
    uint8_t byte_config(uint8_t segment_id, uint8_t byte_id, uint8_t byte_val){
        #if LOG == true
            call("byte_config");
        #endif
        if(sdcard_imgs::is_available == false){
            sdcard_init();
            if(sdcard_imgs::is_available == false){
                #if LOG == true
                    log2ser("sdcard_imgs::is_available: N");
                #endif
                return 0xFF;
            }
        }
        if(!is_cached){
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
        *(config + segment_id * segment_size + byte_id) = byte_val;
        return 0;
    }

};

void local_config_init(){
    #if LOG == true
        call("local_config_init");
    #endif
    if(!local_config::cache_config()){
        local_config::generate_config();
    };
}

#endif