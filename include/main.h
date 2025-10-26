#include "global.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "espidf-helper.h"

#include "../lib/tft/tft.h"
#include "../lib/ctrl/ctrl.h"

// extern spiDevice_t* tftSPI;
extern tftInfo_t * tft;
extern simpUI * sui;

/// SERVICES //////////////////////////////////////////////////////////////////////////////////////

void screenControl(void *pvParameters){
    __entry("screenControl(%p)", pvParameters);
    while(systemStatus __has_flag64(SYS_RUNNING)){
        vTaskDelay(1);
    }
    __exit("screenControl(%p)", pvParameters);
}

void isr0(void* pv){
    btnStatus __set_flag64(NEXT_IMG);
    gpioSetLevel(LED_WHITE_PIN, 1);
    microSecDelay(5000);
    gpioSetLevel(LED_WHITE_PIN, 0);
    microSecDelay(50000);
}

/// INIT //////////////////////////////////////////////////////////////////////////////////////////

void mainInit(){
    __entry("mainInit()");
    systemStatus __set_flag64(SYS_RUNNING);
    createNewSUI(
        &sui,
        __mask64(BTN0_PIN) | __mask64(BTN1_PIN) | 
        __mask64(BTN2_PIN) | __mask64(BTN3_PIN) | __mask64(BTN4_PIN),
        __mask64(LED_YELLOW_PIN) |  __mask64(LED_WHITE_PIN)
    );
    attachISF(sui, BTN0_PIN, isr0);
    ledBlink(__mask64(LED_YELLOW_PIN), 10, 5);
    createTFTScreen(
        &tft, 
        TFT_PIN_CLK, TFT_PIN_MOSI, -1, TFT_PIN_CS, 
        TFT_PIN_RS, TFT_PIN_RST, 10ULL, 50
        #if __SPI_DRIVER_TYPE__ == __HARDWARE__
        , 2, SPI2_HOST
        #endif
    );
    ledBlink(__mask64(LED_YELLOW_PIN), 10, 5);
    tftFillScreen(tft, COLOR_BLACK);
    __exit("mainInit()");
}
