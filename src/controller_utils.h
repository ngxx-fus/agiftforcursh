#ifndef CONTROLLER_UTILS
#define CONTROLLER_UTILS

#include "Arduino.h"
#define VR_X_PIN 35
#define VR_Y_PIN 34
#define SW_PIN 17

#ifndef in_range
#define in_range(x, a, b) (a<=x) && (x<=b)
#endif

static uint16_t sw_pressed_time = 0;

inline uint16_t x_adc_value(){
    return analogRead(VR_X_PIN);
}

inline uint16_t y_adc_value(){
    return analogRead(VR_Y_PIN);
}

inline bool sw_value(){
    return digitalRead(SW_PIN);
}

void controller_init(){
    analogReadResolution(8);
    pinMode(VR_X_PIN, INPUT);
    pinMode(VR_Y_PIN, INPUT);
    pinMode(SW_PIN, PULLUP);
    pinMode(SW_PIN, INPUT);
}

#endif