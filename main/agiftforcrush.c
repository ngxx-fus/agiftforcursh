#include "main.h"

flag64_t            systemStatus;
tftInfo_t *         tft                 = NULL;


void app_main(void)
{
    __entry("app_main()");
    mainInit();
    tftEnterStandby(tft);
    ledBlink(__mask64(LED_YELLOW_PIN), 50, 5);
    tftExitStandby(tft);
    while(systemStatus __has_flag64(SYS_RUNNING)){
        static uint8_t var;
        ledBlink(__mask64(LED_YELLOW_PIN), 100, 5);
        tftFillScreen(tft, colorTable[(var = (var+1) % 40)].value);
        vTaskDelay(1);
    }
    __exit("app_main()");
}
