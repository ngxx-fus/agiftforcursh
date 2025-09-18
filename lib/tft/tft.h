#ifndef __TFT_H__
#define __TFT_H__

#include "../../include/global.h"
#include "tft_utils.h"
#include <stdint.h>
#include <stdbool.h>

/// CONFIGS ///////////////////////////////////////////////////////////////////////////////////////

typedef int8_t      pin_t;      /// Signed byte, -1 meaning unused
typedef uint16_t    xy_t;
typedef uint16_t    color_t;
typedef uint8_t     data8_t;
typedef uint8_t     command8_t;
typedef uint16_t    data16_t;
typedef uint16_t    command16_t;

extern const xy_t screenWidth;
extern const xy_t screenHeight;

#define HIGH                    0x1
#define LOW                     0x0

/// __BIT_BANGING__ : spi bit-banging driver
#define __BIT_BANGING__         0
/// __HARDWARE__    : spi hardware
#define __HARDWARE__            1
/// Set driver type
#define __SPI_DRIVER_TYPE__     __BIT_BANGING__

#define tftSetRS(tft, level)    gpioSetLevel((tft)->rs, level)
#define tftSetRST(tft, level)   gpioSetLevel((tft)->rst, level)

#if __SPI_DRIVER_TYPE__ == __BIT_BANGING__
    #define tftSetCS(tft, level)    gpioSetLevel((tft)->dev->CS, level);
#endif 

#if __SPI_DRIVER_TYPE__ == __HARDWARE__
    #define tftSetCS(tft, level)gpioSetLevel((tft)->CS, level);
#endif

typedef struct colorTable_t {
    const char *name;
    uint16_t value;
} colorTable_t;

#if __SPI_DRIVER_TYPE__ == __BIT_BANGING__
    #include "spi.h"
#endif

#if __SPI_DRIVER_TYPE__ == __HARDWARE__

    #define LCD_HOST        SPI2_HOST
    #define DMA_CHAN        2
    #define SPI_BUS_TYPE    HSPI

#endif 

typedef struct tftInfo_t{
    pin_t rs;       /// SPI-RS (register select) pin
    pin_t rst;      /// SPI-RST (reset) pin
    #if __SPI_DRIVER_TYPE__ == __BIT_BANGING__
        spiDevice_t* dev;   /// SPI device (store clk, miso, mosi, cs)
    #endif 
    #if __SPI_DRIVER_TYPE__ == __HARDWARE__
        pin_t clk;      /// SPI-CLK pin 
        pin_t mosi;     /// SPI-MOSI pin
        pin_t miso;     /// SPI-MISO pin
        pin_t cs;       /// SPI-CS pin
        uint64_t clkFreq;
        uint8_t  clkDutyCycle;
    #endif 
} tftInfo_t;

/// SPI ///////////////////////////////////////////////////////////////////////////////////////////



/// TFT ///////////////////////////////////////////////////////////////////////////////////////////

extern const colorTable_t colorTable[40];

/// @brief Make mono pulse of TFT-RST to reset tft screen
/// @param tft 
void tftReset(tftInfo_t * tft);

/// @brief Intialize the tft screen
/// @param tft 
void tftInitial(tftInfo_t * tft);

/// @brief New TFT screen object
/// @param tft An address of tftInfo_t pointer variable
/// @param clk SPI-CLK pin 
/// @param mosi SPI-MOSI pin
/// @param miso SPI-MISO pin, (currently unused, please set -1)
/// @param cs SPI-CS pin
/// @param rs TFT RS pin (register select)
/// @param rst TFT RST pin (reset)
/// @param clkFreq Frequency of CLK default is 1000000 (Hz)
/// @param clkDutyCycle Duty cycle of CLK, default is 50 (%)
void createTFTScreen(
    tftInfo_t ** tft, 
    pin_t clk, pin_t mosi, pin_t miso, pin_t cs, 
    pin_t rs, pin_t rst, uint64_t clkFreq, uint8_t clkDutyCycle
);

/// @brief Free tft, dev, tft buffer pointer
/// @param tft An address of tftInfo_t pointer variable
void tftFree(tftInfo_t ** tft);

void tftEnterStandby(tftInfo_t * tft);
void tftExitStandby(tftInfo_t * tft);
void tftFillScreen(tftInfo_t * tft, color_t color);


#endif 