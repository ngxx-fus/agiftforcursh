#define SHOW_AUTHOR_MESSAGE     false
#define WIFI_CONNECTION         true
#define SENSORS                 true
#define CONTROLLER              true
#define USB_SERIAL              true
#define TFT_SCREEN              true
#define FIREBASE_RTDB           false
#define SDCARD_RW               true
#define LOG true

#include "main.h"

void setup(){
    #if USB_SERIAL == true
        serial_init();
    #endif
    #if CONTROLLER == true
        controller::custom_isr_handler = sw_isr_service;
        controller_init();
    #endif
    #if SENSORS == true
        sensor_init();
    #endif
    #if FIREBASE_RTDB == true
        firebase_init();
    #endif
    #ifdef TFT_UTILS
        canvas_init();
    #endif
    #if SDCARD_RW == true
        sdcard_init();
    #endif
    screen_mode = enum_SCREEN_MODE::TEST_MODE_BLANK;
}

void loop(){
    /// for emergency return to main loop
    MAIN_LOOP: 
    /// routing screen to each mode
    msg2ser("enter\t", "mode: ", screen_mode);
    switch (screen_mode){
        case enum_SCREEN_MODE::NORMAL_MODE:
            slideshow_mode();
            goto MAIN_LOOP;
        
        case enum_SCREEN_MODE::SETUP_WIFI_MODE: 
            wifi_setup(); 
            goto MAIN_LOOP;
        
        case enum_SCREEN_MODE::SHOW_ENVINFO_MODE:
            run_env_info();
            goto MAIN_LOOP;
        
        case enum_SCREEN_MODE::RESERVED_FEATURE_MODE:
            reserved_feature_mode();
        
        case enum_SCREEN_MODE::TEST_MODE_SCREEN:
            test_mode_screen();
            goto MAIN_LOOP;
        
        case enum_SCREEN_MODE::TEST_MODE_BLANK:
            test_mode_blank();
            goto MAIN_LOOP;
        
        default: 
            error_mode();
    }
}





















