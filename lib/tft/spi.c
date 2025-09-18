#include "spi.h"

#ifdef __SPI_H__

spiDevice_t * createNewTFTSPIDev(pin_t CLK, pin_t MOSI, pin_t MISO, pin_t CS, qword_t clkFreq, byte_t  clkDutyCycle){
    spiDevice_t* dev = (spiDevice_t*)malloc(sizeof(spiDevice_t));
    dev->CLK = __is_positive(CLK)?CLK:SPI_UNUSED;
    dev->MOSI = __is_positive(MOSI)?MOSI:SPI_UNUSED;
    dev->MISO = __is_positive(MISO)?MISO:SPI_UNUSED;
    dev->CS = __is_positive(CS)?CS:SPI_UNUSED;
    dev->clkFreq = __is_positive(clkFreq)?clkFreq:SPI_UNUSED;
    dev->clkPeriod = __is_positive(clkFreq)?(1000000/clkFreq):0;
    dev->clkDutyCycle = __is_positive(clkDutyCycle)?(clkDutyCycle%101):50;

    qword_t outPinMask = 0 ;
    outPinMask __set_flag64(dev->CLK);
    outPinMask __set_flag64(dev->MOSI);
    outPinMask __set_flag64(dev->CS);

    gpio_config_t outPinConf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = outPinMask,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
    };
    gpio_config(&outPinConf);
    
    spiSetCLK(dev, 0x1);
    spiSetMOSI(dev, 0x1);
    spiSetCS(dev, 0x1);
    
    return dev;
}

void spiSendByte(spiDevice_t * dev, byte_t data8){
    // __entry("spiSendByte(%p, %u)", dev, data8);
    for(byte_t mask = 0x80; mask; mask >>= 1){
        // __tag_log("[spiSendByte]", "Sending [%d]", spiBool(data8 & mask));
        spiSetMOSI(dev, spiBool(data8 & mask));
        spiSetCLK(dev, 0x0);
        // __tag_log("spiSendByte", "microSecDelay(%d)",__clkPeriod(dev) * __clkDutyCycle(dev) / 100);
        microSecDelay(__clkPeriod(dev) * __clkDutyCycle(dev) / 100);

        spiSetCLK(dev, 0x1);
        // __tag_log("spiSendByte", "microSecDelay(%d)",__clkPeriod(dev) * (100 - __clkDutyCycle(dev)) / 100);
        microSecDelay(__clkPeriod(dev) * (100 - __clkDutyCycle(dev)) / 100);
    }
    // __exit("spiSendByte()");
}

void spiSendWord(spiDevice_t * dev, word_t data16){
    // __entry("spiSendWord(%p, %u)", dev, data16);
    for(word_t mask = 0x8000; mask; mask >>= 1){
        spiSetMOSI(dev, spiBool(data16 & mask));
        spiSetCLK(dev, 0x0);
        microSecDelay(__clkPeriod(dev) * __clkDutyCycle(dev) / 100);
        
        spiSetCLK(dev, 0x1);
        microSecDelay(__clkPeriod(dev) * (100 - __clkDutyCycle(dev)) / 100);
    }
    // __exit("spiSendWord()");
}

#endif