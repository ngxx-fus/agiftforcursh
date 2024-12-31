#ifndef SDCARD_UTILS_H
#define SDCARD_UTILS_H

#include <vector>

#include "general_utils.h"

#include "FS.h"
#include "SD.h"
#include "SPI.h"

using namespace std;

#ifndef SDCARD_SPI_CS_PIN
    #define SDCARD_SPI_CS_PIN 5
#endif

void sdcard_init(){
    msg2ser("call\t", "sdcard_init");
    if(!SD.begin(SDCARD_SPI_CS_PIN)){
        msg2ser("\t", "SDcard: failed to mount");
        controller::iled_blinky(10);
        if(SD.cardType() == CARD_NONE){
            msg2ser("\t", "SDcard: No SDcard");
            controller::iled_blinky(10);
        }
    }else{
        msg2ser("\t", "SDcard: mounted");
        switch(SD.cardType()){
            case CARD_MMC: msg2ser("\t", "SDcard: Type: MMC"); break;
            case CARD_SD: msg2ser("\t", "SDcard: Type: SDSC"); break;
            case CARD_SDHC: msg2ser("\t", "SDcard: Type: SDHC"); break;
            default: msg2ser("\t", "SDcard: Type: ?");
        }
    }
}

/// @brief Lists the directories on the SD card.
/// @param fs  the main directory’s name
/// @param dirname directory name, corresponds to the microSD card root directory
/// @param levels  the levels to go into the directory
vector<String> sdcard_list_dir(fs::FS &fs, const char *dirname, uint8_t levels)
{
    vector<String> res;

    File root = fs.open(dirname);
    if (!root) return res;
    if (!root.isDirectory())return res;

    File file = root.openNextFile();
    while (file){
        if (file.isDirectory()){
            res.push_back(concatenate("d-", file.name()));
            if (levels)
                sdcard_list_dir(fs, file.name(), levels - 1);
        }
        else
            res.push_back(concatenate("f-", file.name()));
        file = root.openNextFile();
    }
    return res;
}

/// @brief read bytes from a file stored in SDcard
/// @tparam Tdest ```pointer type``` to the first element of block in *dest* 
/// where will store data from src
/// @param fs SD card file system object (usually: ```SD```)
/// @param src path to path stored in SD card
/// @param dest pointer start 
/// @param dest_size a number of bytes will be read
/// @param src_offset where the read process starts  
/// @note FS stands --file.read();for File System, a mechanism for organizing, 
template<class Tdest = char*>
void sdcard_readbytes_file(
    fs::FS &fs, const char *src, 
    Tdest dest, uint64_t dest_size, uint64_t offset = 0
){
    File file = fs.open(src);
    if (!file){
        msg2ser("\t", "SDCard: Read: Can NOT open ", src);
        return;
    }
    if(file.isDirectory() == true){
        msg2ser("\t", "SDCard: Read: Err: isDirectory ");
        return;
    }
    while(offset){file.read(); --offset;}
    while (dest_size){
        *dest = file.read();
        --file.read();
    }
    file.close();
}

/// @brief Writes byte data to a file on the file system.
/// @param fs Reference to the file system object (e.g., SD, SPIFFS, LittleFS).
/// @param path Path to the file to be written to.
/// @param bytes Pointer to the byte data array to be written.
/// @param size_t The number of bytes to write to the file.
/// @param offset Optional parameter specifying the starting point (default is 0). 
/// This allows appending or writing at a specific position in the file.
/// @param raw Optional flag (default is false). When true, writes raw byte data using `file.write()`.
/// If false, writes data using `file.print()`, which might result in formatting (e.g., printing a number as text).
/// @note If the file cannot be opened or the write operation fails, an error message 
/// will be printed to the serial monitor. Ensure the file system is initialized and the 
/// file exists before calling this function. 
/// - When `raw` is true, the function writes data byte-by-byte without conversion, which is more efficient for raw binary data.
/// - When `raw` is false, the function writes data as text, which can introduce extra overhead for binary data.
/// For better performance when writing raw binary data, it is recommended to use the `raw` flag.
template<class Tsrc>
void sdcard_writebytes_file(
    fs::FS &fs, const char *path, 
    const Tsrc bytes, uint64_t size_t, uint64_t offset = 0, 
    bool raw = false
){
    File file = fs.open(path, FILE_WRITE);
    if (!file) {
        msg2ser("\t", "SDCard: Write file: Failed to open");
        return;
    }

    if (raw) {
        // Write raw byte-by-byte data for binary content
        rept(uint64_t, i, 0, size_t-1) {
            file.write(bytes[i]);  // Writing raw byte
        }
    } else {
        // Write byte-by-byte data as text
        rept(uint64_t, i, 0, size_t-1) {
            file.print(bytes[i]);  // Writing byte as text
        }
    }

    file.close();
}

/// @brief remove a file in SD card
/// @param fs SD card file system object (usually: ```SD```)
/// @param path path to file to be removed
inline void sdcard_remove_file(fs::FS &fs, const char *path){
    if (!fs.remove(path))
        msg2ser("\t", "SDCard: Remove dir: failed");
}

/// @brief remove directory
/// @param fs SD card file system object (usually: ```SD```)
/// @param path path to being removed dir
inline void sdcard_remove_dir(fs::FS &fs, const char *path){
    if (!fs.rmdir(path))
        msg2ser("\t", "SDCard: Remove dir: failed");
}


/// @brief Make a directory
/// @param fs SD card file system object (usually: ```SD```)
/// @param path path to the new directory
inline void sdcard_remove_dir(fs::FS &fs, const char *path){
    if (!fs.mkdir(path))
        msg2ser("\t", "SDCard: Make dir: failed");
}

// @brief Renames a file on the SD card.
/// @param fs Reference to the SD card file system object (e.g., SD).
/// @param path_old Path to the existing file to be renamed.
/// @param path_new Path with the new name for the file.
/// @note If the renaming fails, an error message is sent to the serial monitor.
inline void sdcard_rename_file(fs::FS &fs, const char *path_old, const char *path_new){
    if(fs.rename(path_old, path_new))
        msg2ser("\t", "SDCard: Rename ", path_old, " failed");
}

#endif