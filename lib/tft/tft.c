#include "tft.h"

// color_t*                tftBuff;
int16_t                 writeFunctionLevel = 0;
#ifndef ILI9225_LCD_WIDTH
    const xy_t screenWidth = 220;
#else 
    const xy_t screenWidth = ILI9225_LCD_WIDTH;
#endif

#ifndef ILI9225_LCD_HEIGHT
    const xy_t screenHeight = 176;
#else 
    const xy_t screenHeight = ILI9225_LCD_HEIGHT;
#endif

/// SPI ///////////////////////////////////////////////////////////////////////////////////////////

static inline void tftSendCommand(tftInfo_t * tft, command16_t cmd)
{
    tftSetRS(tft, TFT_CMD_MODE);
    #if __SPI_DRIVER_TYPE__ == __BIT_BANGING__
        // spiSetCS(tft->dev, LOW);
        spiSendWord(tft->dev, cmd);
        // spiSetCS(tft->dev, HIGH);
    #endif
    #if __SPI_DRIVER_TYPE__ == __HARDWARE__

    #endif 
}

static inline void tftSendData(tftInfo_t * tft, data16_t data)
{
    tftSetRS(tft, TFT_DATA_MODE);
    #if __SPI_DRIVER_TYPE__ == __BIT_BANGING__
        // spiSetCS(tft->dev, LOW);
        spiSendWord(tft->dev, data);
        // spiSetCS(tft->dev, HIGH);
    #endif
    #if __SPI_DRIVER_TYPE__ == __HARDWARE__

    #endif 
}

void tftCtrlWrite(tftInfo_t * tft, command16_t cmd, data16_t data)
{
    // __entry("tftCtrlWrite(0x%02X, 0x%04X)", cmd, data);
    tftSetCS(tft,       LOW);
    tftSendCommand(tft, cmd);
    tftSendData(tft,    data);
    tftSetCS(tft,    HIGH);
    // __exit("tftCtrlWrite()");
}

/// TFT ///////////////////////////////////////////////////////////////////////////////////////////

void tftReset(tftInfo_t * tft){
    __entry("tftReset(%p)", tft);
    gpioSetLevel(tft->rst, 0x1);
    microSecDelay(10);
    gpioSetLevel(tft->rst, 0x0);
    microSecDelay(50);
    gpioSetLevel(tft->rst, 0x1);
    microSecDelay(10);
    __entry("tftReset()");
}

void tftInitial(tftInfo_t * tft){
    __entry("tftInitial()");
    __log("Check: rs=%d, rst=%d", tft->rs, tft->rst);
    /* Set SS bit and direction output from S528 to S1 */
    tftCtrlWrite(tft, ILI9225_POWER_CTRL1, 0x0000); // Set SAP,DSTB,STB
    tftCtrlWrite(tft, ILI9225_POWER_CTRL2, 0x0000); // Set APON,PON,AON,VCI1EN,VC
    tftCtrlWrite(tft, ILI9225_POWER_CTRL3, 0x0000); // Set BT,DC1,DC2,DC3
    tftCtrlWrite(tft, ILI9225_POWER_CTRL4, 0x0000); // Set GVDD
    tftCtrlWrite(tft, ILI9225_POWER_CTRL5, 0x0000); // Set VCOMH/VCOML voltage
    microSecDelay((40));

    // Power-on sequence
    tftCtrlWrite(tft, ILI9225_POWER_CTRL2, 0x0018); // Set APON,PON,AON,VCI1EN,VC
    tftCtrlWrite(tft, ILI9225_POWER_CTRL3, 0x6121); // Set BT,DC1,DC2,DC3
    tftCtrlWrite(tft, ILI9225_POWER_CTRL4, 0x006F); // Set GVDD   /*007F 0088 */
    tftCtrlWrite(tft, ILI9225_POWER_CTRL5, 0x495F); // Set VCOMH/VCOML voltage
    tftCtrlWrite(tft, ILI9225_POWER_CTRL1, 0x0800); // Set SAP,DSTB,STB
    microSecDelay((10));

    tftCtrlWrite(tft, ILI9225_POWER_CTRL2, 0x103B); // Set APON,PON,AON,VCI1EN,VC
    microSecDelay((50));
    
    tftCtrlWrite(tft, ILI9225_DRIVER_OUTPUT_CTRL, 0x011C); // set the display line number and display direction
    tftCtrlWrite(tft, ILI9225_LCD_AC_DRIVING_CTRL, 0x0100); // set 1 line inversion
    
    // tftCtrlWrite(tft, ILI9225_ENTRY_MODE, 0x1038); // set GRAM write direction and BGR=1.
    tftCtrlWrite(tft, ILI9225_ENTRY_MODE, 0x0040 | (L2R_TopDown << 3)); // set GRAM write direction and BGR=1.
    
    tftCtrlWrite(tft, ILI9225_DISP_CTRL1, 0x0000); // Display off
    tftCtrlWrite(tft, ILI9225_BLANK_PERIOD_CTRL1, 0x0808); // set the back porch and front porch
    tftCtrlWrite(tft, ILI9225_FRAME_CYCLE_CTRL, 0x1100); // set the clocks number per line
    tftCtrlWrite(tft, ILI9225_INTERFACE_CTRL, 0x0000); // CPU interface
    tftCtrlWrite(tft, ILI9225_OSC_CTRL, 0x0D01); // Set Osc  /*0e01*/
    tftCtrlWrite(tft, ILI9225_VCI_RECYCLING, 0x0020); // Set VCI recycling
    tftCtrlWrite(tft, ILI9225_RAM_ADDR_SET1, 0x0000); // RAM Address
    tftCtrlWrite(tft, ILI9225_RAM_ADDR_SET2, 0x0000); // RAM Address

    /* Set GRAM area */
    tftCtrlWrite(tft, ILI9225_GATE_SCAN_CTRL, 0x0000); 
    tftCtrlWrite(tft, ILI9225_VERTICAL_SCROLL_CTRL1, 0x00DB); 
    tftCtrlWrite(tft, ILI9225_VERTICAL_SCROLL_CTRL2, 0x0000); 
    tftCtrlWrite(tft, ILI9225_VERTICAL_SCROLL_CTRL3, 0x0000); 
    tftCtrlWrite(tft, ILI9225_PARTIAL_DRIVING_POS1, 0x00DB); 
    tftCtrlWrite(tft, ILI9225_PARTIAL_DRIVING_POS2, 0x0000); 
    tftCtrlWrite(tft, ILI9225_HORIZONTAL_WINDOW_ADDR1, 0x00AF); 
    tftCtrlWrite(tft, ILI9225_HORIZONTAL_WINDOW_ADDR2, 0x0000); 
    tftCtrlWrite(tft, ILI9225_VERTICAL_WINDOW_ADDR1, 0x00DB); 
    tftCtrlWrite(tft, ILI9225_VERTICAL_WINDOW_ADDR2, 0x0000); 

    /* Set GAMMA curve */
    tftCtrlWrite(tft, ILI9225_GAMMA_CTRL1, 0x0000); 
    tftCtrlWrite(tft, ILI9225_GAMMA_CTRL2, 0x0808); 
    tftCtrlWrite(tft, ILI9225_GAMMA_CTRL3, 0x080A); 
    tftCtrlWrite(tft, ILI9225_GAMMA_CTRL4, 0x000A); 
    tftCtrlWrite(tft, ILI9225_GAMMA_CTRL5, 0x0A08); 
    tftCtrlWrite(tft, ILI9225_GAMMA_CTRL6, 0x0808); 
    tftCtrlWrite(tft, ILI9225_GAMMA_CTRL7, 0x0000); 
    tftCtrlWrite(tft, ILI9225_GAMMA_CTRL8, 0x0A00); 
    tftCtrlWrite(tft, ILI9225_GAMMA_CTRL9, 0x0710); 
    tftCtrlWrite(tft, ILI9225_GAMMA_CTRL10, 0x0710); 

    tftCtrlWrite(tft, ILI9225_DISP_CTRL1, 0x0012); 
    microSecDelay((50)); 
    tftCtrlWrite(tft, ILI9225_DISP_CTRL1, 0x1017);
    __exit("tftInitial()");
}

void createTFTScreen(
    tftInfo_t ** tft, 
    pin_t clk, pin_t mosi, pin_t miso, pin_t cs, 
    pin_t rs, pin_t rst, uint64_t clkFreq, uint8_t clkDutyCycle
){
    __entry("createTFTScreen(%p, %d, %d, %d, %d, %d, %d, %llu, %d)", tft, clk, mosi, miso, cs, rs, rst, clkFreq, clkDutyCycle);
    if(__is_null(tft)) {
        return;
    }
    (*tft) = malloc(sizeof(tftInfo_t));
    (*tft)->rs = rs;
    (*tft)->rst = rst;

    gpio_config_t outPinConf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = __mask64(TFT_PIN_RS) | __mask64(TFT_PIN_RST),
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
    };
    gpio_config(&outPinConf);

    #if __SPI_DRIVER_TYPE__ == __BIT_BANGING__
        (*tft)->dev = createNewTFTSPIDev(clk, mosi, miso, cs, clkFreq, clkDutyCycle);
    #endif
    #if __SPI_DRIVER_TYPE__ == __HARDWARE__
        (*tft)->
    #endif
    tftSetCS((*tft), HIGH);
    tftReset(*tft);
    tftInitial(*tft);
    __exit("createTFTScreen()");
}

void tftFree(tftInfo_t ** tft)          /// Nerver use :>  
{
    __entry("tftFree()");
    if((*tft)->dev) {
        free((*tft)->dev);
        (*tft)->dev = 0;
    }
    if((*tft)){
        free((*tft));
        (*tft) = NULL;
    }
    // if(tftBuff) {
    //     free(tftBuff);
    //     tftBuff = NULL;
    // }
    __exit("tftFree()");
}

void tftEnterStandby(tftInfo_t * tft)
{
    tftCtrlWrite(tft, ILI9225_DISP_CTRL1, 0x0000);
    microSecDelay((50));;
    tftCtrlWrite(tft, ILI9225_POWER_CTRL2, 0x0007);
    microSecDelay((50));;
    tftCtrlWrite(tft, ILI9225_POWER_CTRL1, 0x0A01);
}

void tftExitStandby(tftInfo_t * tft)
{
    tftCtrlWrite(tft, ILI9225_POWER_CTRL1, 0x0A00);
    tftCtrlWrite(tft, ILI9225_POWER_CTRL2, 0x1038);
    microSecDelay((50));;
    tftCtrlWrite(tft, ILI9225_DISP_CTRL1, 0x1017);
}

void tftFillScreen(tftInfo_t * tft, color_t color)
{
    // __entry("tftFillScreen(%d)", color);
    uint32_t total = ILI9225_LCD_WIDTH * ILI9225_LCD_HEIGHT;
    
    tftCtrlWrite(tft, ILI9225_RAM_ADDR_SET1, 0);
    tftCtrlWrite(tft, ILI9225_RAM_ADDR_SET2, 0);
    tftSetCS(tft, 0x0);
    tftSendCommand(tft, ILI9225_GRAM_DATA_REG);
    for (uint32_t i = 0; i < total; i++) {
        tftSendData(tft, color);
    }
    tftSetCS(tft, 0x1);
    // __exit("tftFillScreen()");
}

void tftPutPixel(tftInfo_t * tft, xy_t row, xy_t col, color_t color){
    tftCtrlWrite(tft, ILI9225_RAM_ADDR_SET1, row);
    tftCtrlWrite(tft, ILI9225_RAM_ADDR_SET2, col);
    tftCtrlWrite(tft, ILI9225_GRAM_DATA_REG, color);
}


//// Chat

void tftDrawLine(tftInfo_t *tft, xy_t row0, xy_t col0, xy_t row1, xy_t col1, color_t color)
{
    int dx = abs(col1 - col0);
    int sx = (col0 < col1) ? 1 : -1;
    int dy = -abs(row1 - row0);
    int sy = (row0 < row1) ? 1 : -1;
    int err = dx + dy;

    while (1) {
        // ghi pixel (row0, col0)
        // tftCtrlWrite(tft, ILI9225_RAM_ADDR_SET1, row0);
        // tftCtrlWrite(tft, ILI9225_RAM_ADDR_SET2, col0);
        // tftSendCommand(tft, ILI9225_GRAM_DATA_REG);
        // tftSendData(tft, color);
        tftPutPixel(tft, row0, col0, color);

        if (col0 == col1 && row0 == row1) break;
        int e2 = 2 * err;
        if (e2 >= dy) { err += dy; col0 += sx; }
        if (e2 <= dx) { err += dx; row0 += sy; }
    }
}

void tftDrawEmptyRect(tftInfo_t *tft, xy_t row0, xy_t col0, xy_t row1, xy_t col1, color_t border_color)
{
    tftDrawLine(tft, row0, col0, row0, col1, border_color); // top
    tftDrawLine(tft, row1, col0, row1, col1, border_color); // bottom
    tftDrawLine(tft, row0, col0, row1, col0, border_color); // left
    tftDrawLine(tft, row0, col1, row1, col1, border_color); // right
}

void tftDrawRect(tftInfo_t *tft, xy_t row0, xy_t col0, xy_t row1, xy_t col1,
                 color_t border_color, color_t fill_color)
{
    for (xy_t r = row0; r <= row1; r++) {
        tftCtrlWrite(tft, ILI9225_RAM_ADDR_SET1, r);
        tftCtrlWrite(tft, ILI9225_RAM_ADDR_SET2, col0);
        tftSendCommand(tft, ILI9225_GRAM_DATA_REG);
        for (xy_t c = col0; c <= col1; c++) {
            if (r == row0 || r == row1 || c == col0 || c == col1)
                tftSendData(tft, border_color);
            else
                tftSendData(tft, fill_color);
        }
    }
}

void tftDrawEmptyCircle(tftInfo_t *tft, xy_t rowO, xy_t colO, xy_t radius, color_t border_color)
{
    int f = 1 - radius;
    int ddF_x = 1;
    int ddF_y = -2 * radius;
    int x = 0;
    int y = radius;

    while (x <= y) {
        xy_t px[8] = { rowO + y, rowO - y, rowO + y, rowO - y,
                       rowO + x, rowO - x, rowO + x, rowO - x };
        xy_t py[8] = { colO + x, colO + x, colO - x, colO - x,
                       colO + y, colO + y, colO - y, colO - y };

        for (int i = 0; i < 8; i++) {
            tftPutPixel(tft, px[i], py[i], border_color);
            // tftCtrlWrite(tft, ILI9225_RAM_ADDR_SET1, px[i]);
            // tftCtrlWrite(tft, ILI9225_RAM_ADDR_SET2, py[i]);
            // tftSendCommand(tft, ILI9225_GRAM_DATA_REG);
            // tftSendData(tft, border_color);
        }

        if (f >= 0) { y--; ddF_y += 2; f += ddF_y; }
        x++; ddF_x += 2; f += ddF_x;
    }
}

void tftDrawCircle(tftInfo_t *tft, xy_t rowO, xy_t colO, xy_t radius,
                   color_t border_color, color_t fill_color)
{
    for (int r = -radius; r <= radius; r++) {
        for (int c = -radius; c <= radius; c++) {
            if (r*r + c*c <= radius*radius) {
                // tftCtrlWrite(tft, ILI9225_RAM_ADDR_SET1, rowO + r);
                // tftCtrlWrite(tft, ILI9225_RAM_ADDR_SET2, colO + c);
                // tftSendCommand(tft, ILI9225_GRAM_DATA_REG);
                if (r*r + c*c >= (radius-1)*(radius-1))
                    tftPutPixel(tft, rowO + r, colO + c, border_color);
                    // tftSendData(tft, border_color);
                else
                    tftPutPixel(tft, rowO + r, colO + c, fill_color);
                    // tftSendData(tft, fill_color);
            }
        }
    }
}


/// COLORS ////////////////////////////////////////////////////////////////////////////////////////

const colorTable_t colorTable[40] = {
    {"BLACK",        0x0000},
    {"WHITE",        0xFFFF},
    {"BLUE",         0x001F},
    {"GREEN",        0x07E0},
    {"RED",          0xF800},
    {"NAVY",         0x000F},
    {"DARKBLUE",     0x0011},
    {"DARKGREEN",    0x03E0},
    {"DARKCYAN",     0x03EF},
    {"CYAN",         0x07FF},
    {"TURQUOISE",    0x471A},
    {"INDIGO",       0x4810},
    {"DARKRED",      0x8000},
    {"OLIVE",        0x7BE0},
    {"GRAY",         0x8410},
    {"GREY",         0x8410},
    {"SKYBLUE",      0x867D},
    {"BLUEVIOLET",   0x895C},
    {"LIGHTGREEN",   0x9772},
    {"DARKVIOLET",   0x901A},
    {"YELLOWGREEN",  0x9E66},
    {"BROWN",        0xA145},
    {"DARKGRAY",     0x7BEF},
    {"DARKGREY",     0x7BEF},
    {"SIENNA",       0xA285},
    {"LIGHTBLUE",    0xAEDC},
    {"GREENYELLOW",  0xAFE5},
    {"SILVER",       0xC618},
    {"LIGHTGRAY",    0xC618},
    {"LIGHTGREY",    0xC618},
    {"LIGHTCYAN",    0xE7FF},
    {"VIOLET",       0xEC1D},
    {"AZUR",         0xF7FF},
    {"BEIGE",        0xF7BB},
    {"MAGENTA",      0xF81F},
    {"TOMATO",       0xFB08},
    {"GOLD",         0xFEA0},
    {"ORANGE",       0xFD20},
    {"SNOW",         0xFFDF},
    {"YELLOW",       0xFFE0},
};