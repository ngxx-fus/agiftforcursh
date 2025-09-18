#include "main.h"

flag64_t          systemStatus;
spiDevice_t*      tftSPI              = NULL;


void app_main(void)
{
    __entry("app_main()");
    mainInit();
    tftEnterStandby(tftSPI);
    ledBlink(__mask64(LED_YELLOW_PIN), 30, 10);
    tftExitStandby(tftSPI);
    while(systemStatus __has_flag64(SYS_RUNNING)){
        static uint8_t var;
        ledBlink(__mask64(LED_YELLOW_PIN), 50, 2);
        tftFillScreen(tftSPI, colorTable[(var = (var+1) % 40)].value);
        vTaskDelay(1);
    }
    __exit("app_main()");
}
