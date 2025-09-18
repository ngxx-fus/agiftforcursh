#include "ctrl.h"

flag64_t btnStatus = 0;

void createNewSUI(simpUI ** sui, flag64_t btnMask, flag64_t ledMask){
    if(__is_null(sui)) return;
    (*sui) = malloc(sizeof(simpUI));

    (*sui)->buttonMask  = btnMask;
    (*sui)->ledMask     = ledMask;

    gpio_config_t led_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = ledMask,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
    };
    gpio_config(&led_conf);
    gpio_config_t btn_conf = {
        .intr_type      = GPIO_INTR_NEGEDGE,
        .mode           = GPIO_MODE_INPUT,
        .pin_bit_mask   = btnMask,
        .pull_down_en   = GPIO_PULLDOWN_DISABLE,
        .pull_up_en     = GPIO_PULLUP_DISABLE
    };
    gpio_config(&btn_conf);
    gpio_install_isr_service(ESP_INTR_FLAG_IRAM);
}


void attachISF(simpUI * sui, pin_t pin,  intServiceFn_t isf){
    gpio_isr_handler_add(pin, isf, (void*)sui);
}

void ledBlink(flag64_t ledMask, millisecTime_t msTimeDelay, uint32_t num){
    // __entry("ledBlink(0x%.016x, %llu, %u)", ledMask, msTimeDelay, num);
    do{
        for(uint8_t p = 0; p < sizeof(ledMask)*8; ++p)
            if(ledMask __has_flag64(p)){gpioSetLevel(p, 1);}
        microSecDelay(msTimeDelay*1000/4);
        for(uint8_t p = 0; p < sizeof(ledMask)*8; ++p)
            if(ledMask __has_flag64(p)){gpioSetLevel(p, 1);}
        microSecDelay(msTimeDelay*1000/2);
        for(uint8_t p = 0; p < sizeof(ledMask)*8; ++p)
            if(ledMask __has_flag64(p)){gpioSetLevel(p, 1);}
        microSecDelay(msTimeDelay*1000/4);
        vTaskDelay(1);
    }while(num-- > 0);
    // __exit("ledBlink()");
}