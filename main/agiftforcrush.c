#include "main.h"

flag64_t            systemStatus;
tftInfo_t *         tft                 = NULL;
simpUI *            sui                 = NULL;

void app_main(void)
{
    __entry("app_main()");
    mainInit();
    tftEnterStandby(tft);
    ledBlink(__mask64(LED_YELLOW_PIN), 50, 5);
    tftExitStandby(tft);
    while(systemStatus __has_flag64(SYS_RUNNING)){
        __log("[main_app] Start the loop!");
        static uint8_t var;
        __log("[main_app] Fill background!");
        tftFillScreen(tft, colorTable[(var = (var+1) % 40)].value);
        __log("[main_app] Draw line!");
        tftDrawLine(tft, 1, 1, 175, 219, colorTable[(var+1) % 40].value);
        __log("[main_app] Draw circle!");
        tftDrawEmptyCircle(tft, 70, 100, 50, colorTable[(var+2) % 40].value);
        __log("[main_app] Wait for ir event!");
        while(!(btnStatus __has_flag64(NEXT_IMG)))vTaskDelay(1);
        btnStatus __clr_flag64(NEXT_IMG);
        __log("[main_app] Received an ir event!");
        vTaskDelay(1);
    }
    __exit("app_main()");
}
