#include "ctrl.h"

flag64_t ledStatus;
flag64_t btnStatus;

void buttonAndLEDInit(){
    __entry("buttonAndLEDInit()");
    gpio_config_t led_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = __mask64(LED_YELLOW_PIN) | __mask64(LED_WHITE_PIN),
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
    };
    gpio_config(&led_conf);
    gpio_config_t btn_conf = {
        .intr_type      = GPIO_INTR_NEGEDGE,
        .mode           = GPIO_MODE_INPUT,
        .pin_bit_mask   =   __mask64(BTN0_PIN) | __mask64(BTN1_PIN) |
                            __mask64(BTN2_PIN) | __mask64(BTN3_PIN) |
                            __mask64(BTN4_PIN),
        .pull_down_en   = GPIO_PULLDOWN_DISABLE,
        .pull_up_en     = GPIO_PULLUP_DISABLE
    };
    gpio_config(&btn_conf);
    __exit("buttonAndLEDInit()");
}

void ledBlink(flag64_t ledPinFlag, millisecTime_t msTimeDelay, uint32_t num){
    __entry("ledBlink(0x%.016x, %llu, %u)", ledPinFlag, msTimeDelay, num);
    do{
        for(uint8_t p = 0; p < sizeof(ledPinFlag)*8; ++p)
            if(ledPinFlag __has_flag64(p)){gpioSetLevel(p, 1);}
        microSecDelay(msTimeDelay*1000/4);
        for(uint8_t p = 0; p < sizeof(ledPinFlag)*8; ++p)
            if(ledPinFlag __has_flag64(p)){gpioSetLevel(p, 1);}
        microSecDelay(msTimeDelay*1000/2);
        for(uint8_t p = 0; p < sizeof(ledPinFlag)*8; ++p)
            if(ledPinFlag __has_flag64(p)){gpioSetLevel(p, 1);}
        microSecDelay(msTimeDelay*1000/4);
        vTaskDelay(1);
    }while(num-- > 0);
    __exit("ledBlink()");
}