#ifndef CONTROLLER_UTILS
#define CONTROLLER_UTILS

#include <algorithm>
#include <functional>
using namespace std;
#include <Arduino.h>

#ifndef VR_X_PIN
#define VR_X_PIN 0
#endif

#ifndef VR_Y_PIN
#define VR_Y_PIN 0
#endif

#ifndef SW_PIN
#define SW_PIN 0
#endif

#ifndef in_range
#define in_range(x, a, b) (a<=x) && (x<=b)
#endif
namespace controller{
    static unsigned long last_read = 0;
    static unsigned long last_pressed = 0;
    static bool iled_state = false;

    function<void(void)> custom_isr_handler;

    void toggle_iled_state(){
        iled_state = !iled_state;
        digitalWrite(26, iled_state);
    }

    void iled_blinky(uint16_t times, uint32_t period = 585){
        while(times--){
            digitalWrite(26, LOW); delay(period*494/1000);
            digitalWrite(26, HIGH); delay(1);
            digitalWrite(26, LOW); delay(period*494/1000);
        }
    }

    void isr_handler(){
        toggle_iled_state();
        controller::last_pressed = millis();
        if(controller::custom_isr_handler)
            controller::custom_isr_handler();
        toggle_iled_state();
    }
}
           
/// @brief read ADC value for ADC for X-Axis
/// @return 8-bit ADC for X-Axis
inline uint16_t x_adc_value(){
    controller::last_read = millis();
    return analogRead(VR_X_PIN);
}

/// @brief read ADC value for ADC for Y-Axis
/// @return 8-bit ADC for Y-Axis
inline uint16_t y_adc_value(){
    controller::last_read = millis();
    return analogRead(VR_Y_PIN);
}

/// @brief read logic value for switch
/// @return bool value (FALSE: is pressed)
inline bool sw_value(){
    controller::last_read = millis();
    return digitalRead(SW_PIN);
}

/// @brief Initital controller
/// @note you must manually set ```custom_isr_handler``` 
/// beforce call this function!
void controller_init(){

    msg2ser("call\tcontroller_init: ");
    msg2ser("\t", "analogReadResolution: ", 8);
    msg2ser("\t", "VR_X_PIN: ", VR_X_PIN);
    msg2ser("\t", "VR_Y_PIN: ", VR_Y_PIN);
    msg2ser("\t", "SW_PIN: ", SW_PIN);
    msg2ser("\t", "SW_PIN: ", SW_PIN);

    analogReadResolution(8);
    pinMode(VR_X_PIN, INPUT);
    pinMode(VR_Y_PIN, INPUT);
    pinMode(SW_PIN, INPUT);
    pinMode(SW_PIN, PULLUP);
    pinMode(VR_Y_PIN, PULLDOWN);
    pinMode(VR_X_PIN, PULLDOWN);

    pinMode(0, OUTPUT);


    if(controller::custom_isr_handler){
        msg2ser("\t", "custom_isr_handler: set");
    }else{
        msg2ser("\t", "custom_isr_handler: not set");
    }
    attachInterrupt(
        digitalPinToInterrupt(SW_PIN), 
        controller::isr_handler, 
        FALLING
    );
}

#endif