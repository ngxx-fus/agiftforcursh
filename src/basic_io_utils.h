#ifndef BASIC_IO_UTILS_H
#define BASIC_IO_UTILS_H


#include <algorithm>
#include <functional>
using namespace std;
#include <Arduino.h>

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

#define WAIT_WAIT_200MS delay(200);

namespace basic_io{

    bool                    led0_state = false,
                            led1_state = false,
                            btn0_locked = false,
                            btn1_locked = false,
                            btn2_locked = false,
                            btn3_locked = false,
                            btn0_recent_pressed = false,
                            btn1_recent_pressed = false,
                            btn2_recent_pressed = false,
                            btn3_recent_pressed = false;

    unsigned long           btn0_last_pressed = 0,
                            btn1_last_pressed = 0,
                            btn2_last_pressed = 0,
                            btn3_last_pressed = 0;

    function<void(void)>    btn0_isr_handler,
                            btn1_isr_handler,
                            btn2_isr_handler,
                            btn3_isr_handler;


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

    bool btn0_is_recent_pressed(){
        btn0_locked = true;
        bool tmp = btn0_recent_pressed; 
        btn0_recent_pressed = false;
        btn0_locked = false;
        return tmp;
    }

    bool btn1_is_recent_pressed(){
        btn1_locked = true;
        bool tmp = btn1_recent_pressed; 
        btn1_recent_pressed     = false; 
        btn1_locked = false;
        return tmp;
    }

    bool btn2_is_recent_pressed(){
        btn2_locked = true;
        bool tmp = btn2_recent_pressed; 
        btn2_recent_pressed     = false; 
        btn2_locked = false;
        return tmp;
    }

    bool btn3_is_recent_pressed(){
        btn3_locked = true;
        bool tmp = btn3_recent_pressed; 
        btn3_recent_pressed = false; 
        btn3_locked = false;
        return tmp;
    }

    void isr3(){
        WAIT_WAIT_200MS;
        if(btn3_locked) return; 
        btn3_last_pressed   = millis(); 
        btn3_recent_pressed = true;
        if(btn3_isr_handler) btn3_isr_handler();
        log2ser("isr3");
    }

    void isr2(){
        WAIT_WAIT_200MS;
        if(btn2_locked) return; 
        btn2_last_pressed   = millis();
        btn2_recent_pressed = true;
        if(btn2_isr_handler) btn2_isr_handler();
        log2ser("isr2");
    }

    void isr1(){
        WAIT_WAIT_200MS;
        if(btn1_locked) return; 
        btn1_last_pressed   = millis(); 
        btn1_recent_pressed = true;
        if(btn1_isr_handler) btn1_isr_handler();
        log2ser("isr1");
    }

    void isr0(){
        log2ser("isr0");
        WAIT_WAIT_200MS;
        if(btn0_locked) return; 
        btn0_last_pressed   = millis();
        btn0_recent_pressed = true;
        if(btn0_isr_handler) btn0_isr_handler();
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
        if(basic_io::btn0_isr_handler)
            #if LOG == true
                info("atteched isr at btn0");
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
        if(basic_io::btn1_isr_handler)
            #if LOG == true
                info("atteched isr at btn1");
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
        if(basic_io::btn2_isr_handler)
            #if LOG == true
                info("atteched isr at btn2");
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
        if(basic_io::btn3_isr_handler)
            #if LOG == true
                info("atteched isr at btn3");
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