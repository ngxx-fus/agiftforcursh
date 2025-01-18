#ifndef BASIC_IO_UTILS_H
#define BASIC_IO_UTILS_H


#include <algorithm>
#include <functional>
#include <Arduino.h>
#include "general_utils.h"

using namespace std;

#ifndef LOG
    #define LOG false
#endif
#ifndef BTN0_PIN
    #define BTN0_PIN 0
#endif
#ifndef BTN1_PIN
    #define BTN1_PIN 0
#endif
#ifndef BTN2_PIN
    #define BTN2_PIN 0
#endif
#ifndef BTN3_PIN
    #define BTN3_PIN 0
#endif
#ifndef LED0_PIN
    #define LED0_PIN 0
#endif
#ifndef LED1_PIN
    #define LED1_PIN 0
#endif


#ifndef SAVE_LAST_PRESSED 
    #define SAVE_LAST_PRESSED false
#endif

#ifndef ISR_WAIT_FOR_STABLE
    #define ISR_WAIT_FOR_STABLE delay(200);
#endif

#ifndef CUSTOM_ISR_HANDLER
#define CUSTOM_ISR_HANDLER true
#endif

namespace basic_io{

    bool                        led0_state = false,
                                led1_state = false;

    #if SAVE_LAST_PRESSED == true
        unsigned long           btn0_last_pressed = 0,
                                btn1_last_pressed = 0,
                                btn2_last_pressed = 0,
                                btn3_last_pressed = 0;
    #endif

    #if CUSTOM_ISR_HANDLER == true
        function<void(void)>    btn0_isr_handler,
                                btn1_isr_handler,
                                btn2_isr_handler,
                                btn3_isr_handler;
    #endif


    void toggle_led0_state(){
        led0_state = !led0_state;
        digitalWrite(LED0_PIN, led0_state);
    }

    void toggle_led1_state(){
        led1_state = !led1_state;
        digitalWrite(LED1_PIN, led1_state);
    }

    bool btn0_val(){return digitalRead(BTN0_PIN);}
    bool btn1_val(){return digitalRead(BTN1_PIN);}
    bool btn2_val(){return digitalRead(BTN2_PIN);}
    bool btn3_val(){return digitalRead(BTN3_PIN);}

    bool led0_val(){return led0_state;}
    bool led1_val(){return led1_state;}
    void led0_val(bool state){led0_state = state; digitalWrite(LED0_PIN, state);}
    void led1_val(bool state){led1_state = state; digitalWrite(LED1_PIN, state);}

    void led0_blinky(uint16_t times, uint32_t t_on = 50, uint32_t t_off = 50){
        while(times--){
            digitalWrite(LED0_PIN, LOW); delay(t_off);
            digitalWrite(LED0_PIN, HIGH); delay(t_on);
            digitalWrite(LED0_PIN, LOW);
        }
    }

    void led1_blinky(uint16_t times, uint32_t t_on = 50, uint32_t t_off = 50){
        while(times--){
            digitalWrite(LED1_PIN, LOW); delay(t_off);
            digitalWrite(LED1_PIN, HIGH); delay(t_on);
            digitalWrite(LED1_PIN, LOW);
        }
    }

    void isr3(){
        ISR_WAIT_FOR_STABLE;
        #if SAVE_LAST_PRESSED == true
            btn3_last_pressed   = millis(); 
        #endif
        #if CUSTOM_ISR_HANDLER == true
            if(btn3_isr_handler) btn3_isr_handler();
        #endif
        #if LOG == true
            log2ser("isr3");
        #endif
    }

    void isr2(){
        ISR_WAIT_FOR_STABLE;
        #if SAVE_LAST_PRESSED == true
            btn2_last_pressed   = millis();
        #endif
        #if CUSTOM_ISR_HANDLER == true
            if(btn2_isr_handler) btn2_isr_handler();
        #endif
        #if LOG == true
            log2ser("isr2");
        #endif
    }

    void isr1(){
        ISR_WAIT_FOR_STABLE;
        #if SAVE_LAST_PRESSED == true
            btn1_last_pressed   = millis(); 
        #endif
        #if CUSTOM_ISR_HANDLER == true
            if(btn1_isr_handler) btn1_isr_handler();
        #endif
        #if LOG == true
        log2ser("isr1");
        #endif
    }

    void isr0(){
        ISR_WAIT_FOR_STABLE;
        #if SAVE_LAST_PRESSED == true
            btn0_last_pressed   = millis();
        #endif
        #if CUSTOM_ISR_HANDLER == true
            if(btn0_isr_handler) btn0_isr_handler();
        #endif
        #if LOG == true
            log2ser("isr0");
        #endif
    }

};

void basic_io_init(){
    #if LOG == true
        call("basic_io_init");
    #endif
    #ifdef BTN0_PIN
        #if LOG == true
            info("enable: btn0 at <",BTN0_PIN,">");
            info("enable: btn0: input_pullup");
        #endif
        pinMode(BTN0_PIN, INPUT_PULLUP);
        attachInterrupt(
            digitalPinToInterrupt(BTN0_PIN), 
            basic_io::isr0, 
            FALLING
        );

        #if CUSTOM_ISR_HANDLER == true
            if(basic_io::btn0_isr_handler)
                #if LOG == true
                    info("atteched isr at btn0");
                #endif
        #endif
    #endif
    #ifdef BTN1_PIN
        #if LOG == true
            info("enable: btn1 at <",BTN1_PIN,">");
            info("enable: btn1: input/pullup");
        #endif
        pinMode(BTN1_PIN, INPUT_PULLUP);
        attachInterrupt(
            digitalPinToInterrupt(BTN1_PIN), 
            basic_io::isr1, 
            FALLING
        );
        #if CUSTOM_ISR_HANDLER == true
            if(basic_io::btn1_isr_handler)
                #if LOG == true
                    info("atteched isr at btn1");
                #endif
        #endif
    #endif
    #ifdef BTN2_PIN
        #if LOG == true
            info("enable: btn2 at <",BTN2_PIN,">");
            info("enable: btn2: input/pullup");
        #endif
        pinMode(BTN2_PIN, INPUT_PULLUP);
        attachInterrupt(
            digitalPinToInterrupt(BTN2_PIN), 
            basic_io::isr2, 
            FALLING
        );
        #if CUSTOM_ISR_HANDLER == true
            if(basic_io::btn2_isr_handler)
                #if LOG == true
                    info("atteched isr at btn2");
                #endif
        #endif
    #endif
    #ifdef BTN3_PIN
        #if LOG == true
            info("enable: btn3 at <",BTN3_PIN,">");
            info("enable: btn3: input/pullup");
        #endif
        pinMode(BTN3_PIN, INPUT_PULLUP);
        attachInterrupt(
            digitalPinToInterrupt(BTN3_PIN), 
            basic_io::isr3, 
            FALLING
        );
        #if CUSTOM_ISR_HANDLER == true
            if(basic_io::btn3_isr_handler)
                #if LOG == true
                    info("atteched isr at btn3");
                #endif
        #endif
    #endif
    #ifdef LED0_PIN
        #if LOG == true
            info("enable: led<0> at <",LED0_PIN,">");
        #endif
        pinMode(LED0_PIN, OUTPUT);
    #endif
    #ifdef LED1_PIN
        #if LOG == true
            info("enable: led<1> at <",LED1_PIN,">");
        #endif
        pinMode(LED1_PIN, OUTPUT);
    #endif
}

#endif