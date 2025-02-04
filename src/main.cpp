#include "main.h"

void setup(){
    #if USB_SERIAL == true
        serial_init();
    #endif
    #if BASIC_IO == true
        basic_io_init();
    #endif
    #if SENSORS == true
        sensor_init();
    #endif
    #if TFT_SCREEN == true
        canvas_init();
    #endif
    #if HARDWARE_TEST == false && SOFTWARE_TEST == false
        #if WIFI_CONNECTION == true \
            && SKIP_WIFI_SETUP == false
            wifi_setup();
        #endif
        #if SDCARD_RW == true
            sdcard_init();
        #endif
        #if LOCAL_CONFIG == true
            single_screen_color_and_text_line(1, "[config]", 0xFFFF, 0x0, false);
            single_screen_color_and_text_line(2, "loading...", 0xFFFF, 0x0, true, false);
            local_config_init();
        #endif
    #endif
    
    screen_mode = enum_SCREEN_MODE::NORMAL_MODE;
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
                #if WIFI_CONNECTION == true
                    wifi_setup(); 
                #endif
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
                goto MAIN_LOOP;
            
            default: 
                error_mode();
        }
    #endif

    #if HARDWARE_TEST == true
        log2ser("btn4: ", digitalRead(BTN4_PIN));
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













