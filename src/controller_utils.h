#ifndef CONTROLLER_UTILS
#define CONTROLLER_UTILS

#include <algorithm>
#include <functional>
using namespace std;
#include <Arduino.h>

#ifndef VR_X_PIN
#define VR_X_PIN 35
#endif

#ifndef VR_Y_PIN
#define VR_Y_PIN 34
#endif

#ifndef SW_PIN
#define SW_PIN 17
#endif

#ifndef in_range
#define in_range(x, a, b) (a<=x) && (x<=b)
#endif

static unsigned long last_used = 0;

/// @brief read ADC value for ADC for X-Axis
/// @return 8-bit ADC for X-Axis
inline uint16_t x_adc_value(){
    last_used = millis();
    return analogRead(VR_X_PIN);
}

/// @brief read ADC value for ADC for Y-Axis
/// @return 8-bit ADC for Y-Axis
inline uint16_t y_adc_value(){
    last_used = millis();
    return analogRead(VR_Y_PIN);
}

/// @brief read logic value for switch
/// @return bool value (FALSE: is pressed)
inline bool sw_value(){
    last_used = millis();
    return digitalRead(SW_PIN);
}

void toggle_led(){
    msg2ser("call\t", "toggle_led");
    digitalWrite(26, ~digitalRead(26));
}

/// @brief Initital controller
/// @param isr_handler A function, called when sw is pressed 
void controller_init( void(&isr_handler)() ){

    msg2ser("call\tcontroller_init: ");
    msg2ser("\t", "analogReadResolution: ", 8);
    msg2ser("\t", "VR_X_PIN: ", VR_X_PIN);
    msg2ser("\t", "VR_Y_PIN: ", VR_Y_PIN);
    msg2ser("\t", "SW_PIN: ", SW_PIN);

    analogReadResolution(8);
    pinMode(VR_X_PIN, INPUT);
    pinMode(VR_Y_PIN, INPUT);
    pinMode(SW_PIN, INPUT);
    pinMode(SW_PIN, PULLUP);
    if(isr_handler){
        msg2ser("\t", "isr_handler: set");
        attachInterrupt(digitalPinToInterrupt(SW_PIN), isr_handler, FALLING);
    }
}

#endif