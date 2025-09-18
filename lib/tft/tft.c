#include "tft.h"

color_t*                tftBuff;
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

static inline void tftSendCommand(spiDevice_t * tftSPI, command16_t cmd)
{
    // spiSetCS(tftSPI, 0x0);
    gpioSetLevel(TFT_PIN_RS, TFT_CMD_MODE);
    spiSendWord(tftSPI, cmd);
    // spiSetCS(tftSPI, 0x1);
}

static inline void tftSendData(spiDevice_t * tftSPI, data16_t data)
{
    // spiSetCS(tftSPI, 0x0);
    gpioSetLevel(TFT_PIN_RS, TFT_DATA_MODE);  // Data mode (RS=1)
    spiSendWord(tftSPI, data);
    // spiSetCS(tftSPI, 0x1);
}

void tftCtrlWrite(spiDevice_t * tftSPI, command16_t cmd, data16_t data)
{
    __entry("tftCtrlWrite(0x%02X, 0x%04X)", cmd, data);
    // spiStartTransaction(tftSPI); 
    spiSetCS(tftSPI, 0x0);
    tftSendCommand(tftSPI, cmd);
    tftSendData(tftSPI, data);
    // spiEndTransaction(tftSPI);
    spiSetCS(tftSPI, 0x1);
    __entry("tftCtrlWrite(_)");
}

/// TFT ///////////////////////////////////////////////////////////////////////////////////////////

void configTFTGPIO(pin_t RS, pin_t RST){
    __entry("configTFTGPIO(%d, %d)", RS, RST);
    gpio_config_t outPinConf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = __mask64(RS) | __mask64(RST),
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
    };
    gpio_config(&outPinConf);
    __exit("configTFTGPIO()");
}

void tftReset(spiDevice_t* tftSPI){
    __entry("tftReset(%p)", tftSPI);
    gpioSetLevel(TFT_PIN_RST, 0x1);
    microSecDelay(10);
    gpioSetLevel(TFT_PIN_RST, 0x0);
    microSecDelay(40);
    gpioSetLevel(TFT_PIN_RST, 0x1);
    microSecDelay(50);
    __entry("tftReset()");
}

void tftInitial(spiDevice_t ** tftSPI){
    __entry("tftInitial()");
    (*tftSPI) = createNewTFTSPIDev(
        TFT_PIN_CLK,
        TFT_PIN_MOSI,
        -1,
        TFT_PIN_CS,
        10000000,
        0
    );
    
    configTFTGPIO(TFT_PIN_RS, TFT_PIN_RST);

    tftReset(*tftSPI);

    __tag_log("tftInitial", "Set SS bit and direction output from S528 to S1");
    /* Set SS bit and direction output from S528 to S1 */
    // spiStartTransaction((*tftSPI));
    tftCtrlWrite((*tftSPI), ILI9225_POWER_CTRL1, 0x0000); // Set SAP,DSTB,STB
    tftCtrlWrite((*tftSPI), ILI9225_POWER_CTRL2, 0x0000); // Set APON,PON,AON,VCI1EN,VC
    tftCtrlWrite((*tftSPI), ILI9225_POWER_CTRL3, 0x0000); // Set BT,DC1,DC2,DC3
    tftCtrlWrite((*tftSPI), ILI9225_POWER_CTRL4, 0x0000); // Set GVDD
    tftCtrlWrite((*tftSPI), ILI9225_POWER_CTRL5, 0x0000); // Set VCOMH/VCOML voltage
    // spiEndTransaction(*tftSPI);
    microSecDelay((40));

    // Power-on sequence
    __tag_log("tftInitial", "Power-on sequence");
    // spiStartTransaction((*tftSPI));
    tftCtrlWrite((*tftSPI), ILI9225_POWER_CTRL2, 0x0018); // Set APON,PON,AON,VCI1EN,VC
    tftCtrlWrite((*tftSPI), ILI9225_POWER_CTRL3, 0x6121); // Set BT,DC1,DC2,DC3
    tftCtrlWrite((*tftSPI), ILI9225_POWER_CTRL4, 0x006F); // Set GVDD   /*007F 0088 */
    tftCtrlWrite((*tftSPI), ILI9225_POWER_CTRL5, 0x495F); // Set VCOMH/VCOML voltage
    tftCtrlWrite((*tftSPI), ILI9225_POWER_CTRL1, 0x0800); // Set SAP,DSTB,STB
    // spiEndTransaction(*tftSPI);
    microSecDelay((10));

    tftCtrlWrite((*tftSPI), ILI9225_POWER_CTRL2, 0x103B); // Set APON,PON,AON,VCI1EN,VC
    microSecDelay((50));
    
    // spiStartTransaction((*tftSPI));
    tftCtrlWrite((*tftSPI), ILI9225_DRIVER_OUTPUT_CTRL, 0x011C); // set the display line number and display direction
    tftCtrlWrite((*tftSPI), ILI9225_LCD_AC_DRIVING_CTRL, 0x0100); // set 1 line inversion
    tftCtrlWrite((*tftSPI), ILI9225_ENTRY_MODE, 0x1038); // set GRAM write direction and BGR=1.
    tftCtrlWrite((*tftSPI), ILI9225_DISP_CTRL1, 0x0000); // Display off
    tftCtrlWrite((*tftSPI), ILI9225_BLANK_PERIOD_CTRL1, 0x0808); // set the back porch and front porch
    tftCtrlWrite((*tftSPI), ILI9225_FRAME_CYCLE_CTRL, 0x1100); // set the clocks number per line
    tftCtrlWrite((*tftSPI), ILI9225_INTERFACE_CTRL, 0x0000); // CPU interface
    tftCtrlWrite((*tftSPI), ILI9225_OSC_CTRL, 0x0D01); // Set Osc  /*0e01*/
    tftCtrlWrite((*tftSPI), ILI9225_VCI_RECYCLING, 0x0020); // Set VCI recycling
    tftCtrlWrite((*tftSPI), ILI9225_RAM_ADDR_SET1, 0x0000); // RAM Address
    tftCtrlWrite((*tftSPI), ILI9225_RAM_ADDR_SET2, 0x0000); // RAM Address
    // spiEndTransaction(*tftSPI);

    /* Set GRAM area */
    __tag_log("tftInitial", "Set GRAM area");
    // spiStartTransaction((*tftSPI));
    tftCtrlWrite((*tftSPI), ILI9225_GATE_SCAN_CTRL, 0x0000); 
    tftCtrlWrite((*tftSPI), ILI9225_VERTICAL_SCROLL_CTRL1, 0x00DB); 
    tftCtrlWrite((*tftSPI), ILI9225_VERTICAL_SCROLL_CTRL2, 0x0000); 
    tftCtrlWrite((*tftSPI), ILI9225_VERTICAL_SCROLL_CTRL3, 0x0000); 
    tftCtrlWrite((*tftSPI), ILI9225_PARTIAL_DRIVING_POS1, 0x00DB); 
    tftCtrlWrite((*tftSPI), ILI9225_PARTIAL_DRIVING_POS2, 0x0000); 
    tftCtrlWrite((*tftSPI), ILI9225_HORIZONTAL_WINDOW_ADDR1, 0x00AF); 
    tftCtrlWrite((*tftSPI), ILI9225_HORIZONTAL_WINDOW_ADDR2, 0x0000); 
    tftCtrlWrite((*tftSPI), ILI9225_VERTICAL_WINDOW_ADDR1, 0x00DB); 
    tftCtrlWrite((*tftSPI), ILI9225_VERTICAL_WINDOW_ADDR2, 0x0000); 
    // spiEndTransaction(*tftSPI);

    /* Set GAMMA curve */
    __tag_log("tftInitial", "Set GAMMA curve");
    // spiStartTransaction((*tftSPI));
    tftCtrlWrite((*tftSPI), ILI9225_GAMMA_CTRL1, 0x0000); 
    tftCtrlWrite((*tftSPI), ILI9225_GAMMA_CTRL2, 0x0808); 
    tftCtrlWrite((*tftSPI), ILI9225_GAMMA_CTRL3, 0x080A); 
    tftCtrlWrite((*tftSPI), ILI9225_GAMMA_CTRL4, 0x000A); 
    tftCtrlWrite((*tftSPI), ILI9225_GAMMA_CTRL5, 0x0A08); 
    tftCtrlWrite((*tftSPI), ILI9225_GAMMA_CTRL6, 0x0808); 
    tftCtrlWrite((*tftSPI), ILI9225_GAMMA_CTRL7, 0x0000); 
    tftCtrlWrite((*tftSPI), ILI9225_GAMMA_CTRL8, 0x0A00); 
    tftCtrlWrite((*tftSPI), ILI9225_GAMMA_CTRL9, 0x0710); 
    tftCtrlWrite((*tftSPI), ILI9225_GAMMA_CTRL10, 0x0710); 
    // spiEndTransaction(*tftSPI);

    tftCtrlWrite((*tftSPI), ILI9225_DISP_CTRL1, 0x0012); 
    microSecDelay((50)); 
    tftCtrlWrite((*tftSPI), ILI9225_DISP_CTRL1, 0x1017);
    spiEndTransaction((*tftSPI));
    __exit("tftInitial()");
}

void tftFree(void)          /// Nerver use :>  
{
    __entry("tftFree()");
    free(tftBuff);
    __exit("tftFree()");
}

void tftEnterStandby(spiDevice_t * tftSPI)
{
    tftCtrlWrite(tftSPI, ILI9225_DISP_CTRL1, 0x0000);
    microSecDelay((50));;
    tftCtrlWrite(tftSPI, ILI9225_POWER_CTRL2, 0x0007);
    microSecDelay((50));;
    tftCtrlWrite(tftSPI, ILI9225_POWER_CTRL1, 0x0A01);
}

void tftExitStandby(spiDevice_t * tftSPI)
{
    tftCtrlWrite(tftSPI, ILI9225_POWER_CTRL1, 0x0A00);
    tftCtrlWrite(tftSPI, ILI9225_POWER_CTRL2, 0x1038);
    microSecDelay((50));;
    tftCtrlWrite(tftSPI, ILI9225_DISP_CTRL1, 0x1017);
}

void tftFillScreen(spiDevice_t * tftSPI, color_t color)
{
    __entry("tftFillScreen(%d)", color);
    uint32_t total = ILI9225_LCD_WIDTH * ILI9225_LCD_HEIGHT;
    
    tftCtrlWrite(tftSPI, ILI9225_RAM_ADDR_SET1, 0);
    tftCtrlWrite(tftSPI, ILI9225_RAM_ADDR_SET2, 0);
    spiSetCS(tftSPI, 0x0);
    tftSendCommand(tftSPI, ILI9225_GRAM_DATA_REG);
    for (uint32_t i = 0; i < total; i++) {
        tftSendData(tftSPI, color);
    }
    spiSetCS(tftSPI, 0x1);
    __exit("tftFillScreen()");
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