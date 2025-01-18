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

bool sdcard_is_available = true;

void sdcard_init(){
    #if LOG == true
        call("sdcard_init: ");
    #endif
    SPI.setFrequency(UINT32_MAX);
    call( "sdcard_init");
    if(!SD.begin(SDCARD_SPI_CS_PIN)){
        log2ser( "SDcard: failed to mount");
        controller::iled_blinky(10);
        if(SD.cardType() == CARD_NONE){
            sdcard_is_available = false;
            log2ser( "SDcard: No SDcard");
            #if BASIC_IO == true
                basic_io::led0_blinky(5, 15, 30);
            #endif
            #if CONTROLLER == true
                controller::iled_blinky(10);
            #endif
        }
    }else{
        log2ser( "SDcard: mounted");
        switch(SD.cardType()){
            case CARD_MMC: log2ser( "SDcard: Type: MMC"); break;
            case CARD_SD: log2ser( "SDcard: Type: SDSC"); break;
            case CARD_SDHC: log2ser( "SDcard: Type: SDHC"); break;
            default: log2ser( "SDcard: Type: ?");
        }
    }
}

/// @brief Lists the directories on the SD card.
/// @param fs  the main directory’s name
/// @param dirname directory name, corresponds to the microSD card root directory
/// @param levels  the levels to go into the directory
/// @note  only return file/dir on same levels
vector<String> sdcard_list_dir(fs::FS &fs, const char *dirname, uint8_t levels)
{
    #if LOG == true
        call("sdcard_list_dir");
    #endif

    vector<String> res;

    File root = fs.open(dirname);
    if (!root) return res;
    if (!root.isDirectory())return res;

    File file = root.openNextFile();
    while (file){
        if (file.isDirectory()){
            if(!levels == 0) res.push_back(file.name());
            if (levels)
                sdcard_list_dir(fs, file.name(), levels - 1);
        }
        else
            if(levels == 0) res.push_back(file.name());
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
    Tdest dest, uint64_t dest_size, uint64_t offset = 0,
    bool readall = false
){
    File file = fs.open(src);
    if (!file){
        log2ser( "SDCard: Read: Can NOT open ", src);
        return;
    }
    if(file.isDirectory() == true){
        log2ser( "SDCard: Read: Err: isDirectory ");
        return;
    }
    while(offset){file.read(); --offset;}
    if(readall == false)
        while (--dest_size) *(dest++) = file.read();
    else
        while(file.available()) *(dest++) = file.read();
    file.close();
}

/// @brief read bytes from a file stored in SDcard
/// @param fs SD card file system object (usually: ```SD```)
/// @param src path to path stored in SD card
/// @param read_size a number of bytes will be read
/// @param byte_process the process will process with each read byte
/// @param offset where the read process starts
void sdcard_readbytes_file(
    fs::FS &fs, const char *src,
    uint64_t read_size,
    function<void(int)> byte_process,
    uint64_t offset = 0
){
    File file = fs.open(src);
    if (!file){
        log2ser( "SDCard: Read: Can NOT open ", src);
        return;
    }
    if(file.isDirectory() == true){
        log2ser( "SDCard: Read: Err: isDirectory ");
        return;
    }
    
    while(offset--)     file.read();
    while (read_size--) byte_process(file.read());
    
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
        log2ser( "SDCard: Write file: Failed to open");
        return;
    }

    if (raw) {
        /// Write raw byte-by-byte data for binary content
        rept(uint64_t, i, 0, size_t-1) {
            file.write(bytes[i]);  /// Writing raw byte
        }
    } else {
        /// Write byte-by-byte data as text
        rept(uint64_t, i, 0, size_t-1) {
            file.print(bytes[i]);  /// Writing byte as text
        }
    }

    file.close();
}

/// @brief Reads a 565-format image from a binary file and stores it in a provided buffer.
/// @param fs Reference to the file system object (e.g., SD, SPIFFS, LittleFS).
/// @param img_path Path to the binary file containing the 565-format image.
/// @param img Pointer to a buffer where the image pixels will be stored. The buffer must have enough space for the specified size.
/// @param size The number of pixels to read from the file.
/// @param offset The number of pixels to skip before starting to read. Defaults to 0.
/// @note The 565-format is a 16-bit RGB format where:
///       - 5 bits are for red.
///       - 6 bits are for green.
///       - 5 bits are for blue.
///       Each pixel is stored as two bytes (little-endian format, LSB first).
///       Ensure the file exists, and the buffer provided has sufficient space for `size` pixels.
///       The function reads the image pixel by pixel and combines the two bytes to form each 16-bit pixel.
void read_and_insert_565format_image(
    fs::FS &fs, String img_path,
    POINT<uint16_t> pos, uint16_t w, uint16_t h
){
    call( "read_and_insert_565format_image");
    File file = fs.open(img_path, FILE_READ);
    if (!file) {
        log2ser( "SDCard: Read 565f img: Failed to open");
        return;
    }

    uint8_t  buf16[2];
    uint16_t r, c;
    POINT<uint16_t> ins_pos;

    rept(uint16_t, i, 0, w*h-1){
        file.read(buf16, 2);
        uint16_t pixel =  ((uint16_t(buf16[0])&0xFF) << 8) | (uint16_t(buf16[1])&0xFF);
        r = uint16_t(i/w), c = uint16_t(i%w); 
        ins_pos.X() = pos.X() + r;
        ins_pos.Y() = pos.Y() + c;
        canvas.set_pixel(ins_pos, pixel);
    }

    file.close();  /// Close the file after reading
}

/// @brief remove a file in SD card
/// @param fs SD card file system object (usually: ```SD```)
/// @param path path to file to be removed
inline void sdcard_remove_file(fs::FS &fs, const char *path){
    if (!fs.remove(path))
        log2ser( "SDCard: Remove dir: failed");
}

/// @brief remove directory
/// @param fs SD card file system object (usually: ```SD```)
/// @param path path to being removed dir
inline void sdcard_remove_dir(fs::FS &fs, const char *path){
    if (!fs.rmdir(path))
        log2ser( "SDCard: Remove dir: failed");
}


/// @brief Make a directory
/// @param fs SD card file system object (usually: ```SD```)
/// @param path path to the new directory
inline void sdcard_make_dir(fs::FS &fs, const char *path){
    if (!fs.mkdir(path))
        log2ser( "SDCard: Make dir: failed");
}

/// @brief Renames a file on the SD card.
/// @param fs Reference to the SD card file system object (e.g., SD).
/// @param path_old Path to the existing file to be renamed.
/// @param path_new Path with the new name for the file.
/// @note If the renaming fails, an error message is sent to the serial monitor.
inline void sdcard_rename_file(fs::FS &fs, const char *path_old, const char *path_new){
    if(fs.rename(path_old, path_new))
        log2ser( "SDCard: Rename ", path_old, " failed");
}

#endif