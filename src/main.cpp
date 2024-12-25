#define SHOW_AUTHOR_MESSAGE false
#define WIFI_CONNECTION true
#define SENSORS true
#define CONTROLLER true
#define USB_SERIAL true
#define TFT_SCREEN true
#define LOG true

#include "main.h"

void setup(){
    // pinMode(26, OUTPUT);
    #if USB_SERIAL == true
        serial_init();
    #endif
    #if CONTROLLER == true
        controller_init(sw_isr_service);
    #endif
    #if SENSORS == true
        sensor_init();
    #endif
    #ifdef TFT_UTILS
        canvas_init();
    #endif
    screen_mode = enum_SCREEN_MODE::SETUP_WIFI_MODE;
}

void loop(){
    /// for emergency return to main loop
    MAIN_LOOP: 
    /// routing screen to each mode
    msg2ser("enter\tmode: ", screen_mode);
    
    switch (screen_mode){
        case enum_SCREEN_MODE::NORMAL_MODE:
            canvas.clear(); 
            canvas.show();
            while(screen_mode == NORMAL_MODE) 
                digitalWrite(26, !bool(digitalRead(26)));
            break;
        case enum_SCREEN_MODE::SETUP_WIFI_MODE: 
            wifi_setup(); 
            break;
        case enum_SCREEN_MODE::SHOW_ENVINFO_MODE:
            run_env_info(15);
            break;
        default: 
            error_mode(); while(0x1);
    }
}





















