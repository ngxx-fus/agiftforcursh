#ifndef __TFT_H__
#define __TFT_H__

#include "../../include/global.h"
#include "tft_utils.h"
#include <stdint.h>
#include <stdbool.h>


typedef uint16_t    xy_t;
typedef uint16_t    color_t;
typedef uint8_t     data8_t;
typedef uint8_t     command8_t;
typedef uint16_t    data16_t;
typedef uint16_t    command16_t;

extern const xy_t screenWidth;
extern const xy_t screenHeight;

/// SPI ///////////////////////////////////////////////////////////////////////////////////////////

#include "spi.h"

#define LCD_HOST        SPI2_HOST
#define DMA_CHAN        2

#define SPI_BUS_TYPE    HSPI

#define TFT_PIN_MOSI    13
#define TFT_PIN_CLK     14
#define TFT_PIN_CS      15
#define TFT_PIN_RST     2
#define TFT_PIN_RS      4

extern spiDevice_t* tftSPI;

/// TFT ///////////////////////////////////////////////////////////////////////////////////////////


typedef struct colorTable_t {
    const char *name;
    uint16_t value;
} colorTable_t;

extern const colorTable_t colorTable[40];

// extern color_t* tftBuff;

void tftInitial(spiDevice_t ** tftSPI);
void tftEnterStandby(spiDevice_t * tftSPI);
void tftExitStandby(spiDevice_t * tftSPI);
void tftFillScreen(spiDevice_t * tftSPI, color_t color);

#endif 