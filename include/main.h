#include "global.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "../lib/tft/tft.h"
#include "../lib/ctrl/ctrl.h"

extern spiDevice_t* tftSPI;

/// INIT //////////////////////////////////////////////////////////////////////////////////////////

void mainInit(){
    __entry("mainInit()");
    esp_log_level_set("spi_master", ESP_LOG_VERBOSE);
    systemStatus __set_flag64(SYS_RUNNING);
    ledBlink(__mask64(LED_YELLOW_PIN), 100, 0);
    buttonAndLEDInit();
    ledBlink(__mask64(LED_YELLOW_PIN), 100, 0);
    tftInitial(&tftSPI);
    ledBlink(__mask64(LED_YELLOW_PIN), 100, 0);
    // tftFillScreen(COLOR_SKYBLUE);
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