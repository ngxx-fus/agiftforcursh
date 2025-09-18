#include "global.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "../lib/tft/tft.h"
#include "../lib/ctrl/ctrl.h"

// extern spiDevice_t* tftSPI;
extern tftInfo_t * tft;

/// INIT //////////////////////////////////////////////////////////////////////////////////////////

void mainInit(){
    __entry("mainInit()");
    systemStatus __set_flag64(SYS_RUNNING);
    buttonAndLEDInit();
    ledBlink(__mask64(LED_YELLOW_PIN), 10, 5);
    createTFTScreen(&tft, TFT_PIN_CLK, TFT_PIN_MOSI, -1, TFT_PIN_CS, TFT_PIN_RS, TFT_PIN_RST, 1000000ULL, 50);
    ledBlink(__mask64(LED_YELLOW_PIN), 10, 5);
    tftFillScreen(tft, COLOR_BLACK);
    __exit("mainInit()");
}

/// SERVICES //////////////////////////////////////////////////////////////////////////////////////

void screenControl(void *pvParameters){
    __entry("screenControl(%p)", pvParameters);
    while(systemStatus __has_flag64(SYS_RUNNING)){
        vTaskDelay(1);
    }
    __exit("screenControl(%p)", pvParameters);
}