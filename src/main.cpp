#define LOG                     true
#define SHOW_AUTHOR_MESSAGE     false
#define WIFI_CONNECTION         true
#define USB_SERIAL              true
#define TFT_SCREEN              true
#define SENSORS                 true
#define FIREBASE_RTDB           true
#define SDCARD_RW               true
#define SAVE_LAST_PRESSED       true
#define CUSTOM_ISR_HANDLER      false
#define SOFTWARE_TEST           false
#define HARDWARE_TEST           false
#define BASIC_IO                true
#define LOCAL_CONFIG            true

#include "main.h"

void setup(){
    #if USB_SERIAL == true
        serial_init();
    #endif
    #if BASIC_IO == true && HARDWARE_TEST == false
        basic_io_init();
    #endif
    #if SENSORS == true
        sensor_init();
    #endif
    #if TFT_SCREEN == true
        canvas_init();
    #endif
    #if WIFI_CONNECTION == true
        wifi_setup();
    #endif
    #if SDCARD_RW == true
        sdcard_init();
    #endif
    #if LOCAL_CONFIG == true
        config_init();
    #endif
}

void loop(){
    #if HARDWARE_TEST == false \
        && SOFTWARE_TEST == false
        /// for emergency return to main loop
        MAIN_LOOP: 
        /// routing screen to each mode
        log2ser("switched mode: ", screen_mode);
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
                goto MAIN_LOOP;
            
            case enum_SCREEN_MODE::TEST_MODE_SCREEN:
                test_mode_screen();
                goto MAIN_LOOP;
            
            case enum_SCREEN_MODE::TEST_MODE_BLANK:
                test_mode_blank();
                goto MAIN_LOOP;
            
            default: 
                error_mode();
        }
    #endif

    #if SOFTWARE_TEST == true
        slideshow_mode();
    #endif

    #if HARDWARE_TEST == true
        log2ser("btn3: ", digitalRead(BTN3_PIN));
        log2ser("btn2: ", digitalRead(BTN2_PIN));
        log2ser("btn1: ", digitalRead(BTN1_PIN));
        log2ser("btn0: ", digitalRead(BTN0_PIN));

        basic_io::toggle_led0_state(); delay(187);
        basic_io::toggle_led1_state(); delay(231);

        #if TFT_SCREEN == true
            test_mode_screen();
        #endif
    #endif
}













