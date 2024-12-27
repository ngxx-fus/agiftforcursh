#include <Arduino.h>
#include <algorithm>
using namespace std;

#define  elif else if
/// >>>>>>>>>>>>>>>>>>>>> PIN DEFINE >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

#define VR_X_PIN 35
#define VR_Y_PIN 34
#define SW_PIN 17

#define DHT_TYPE DHT11
#define DHT_PIN 16

/// >>>>>>>>>>>>>>>>>>>>> HEADER INCLUDES >>>>>>>>>>>>>>>>>>>>>>>>>>
#include "images.h"

#if USB_SERIAL == true
    #include "serial_utils.h"
#endif

#if CONTROLLER == true
    #include "controller_utils.h"
#endif

#include "tft_utils.h"

#if SENSORS == true
    #include "sensor_utils.h"
#endif

#if WIFI_CONNECTION == true
    #include "wifi_utils.h"
#endif


/// >>>>>>>>>>>>>>>>>>>> OTHERS DEFINITIONS >>>>>>>>>>>>>>>>>>>>>>>>

/// @brief show error screen
void error_mode(){
    canvas.refill(0xc0e5);
    uint16_t err_row = 18;
    canvas.insert_text(POINT<>(18, 55), "ERROR", 0xffff);
    canvas.insert_text(POINT<>(err_row+=18, 5), "Hello!", 0xffff);
    canvas.insert_text(POINT<>(err_row+=18, 5), "An error occured!", 0xffff);
    canvas.insert_text(POINT<>(err_row+=18, 5), "Please press RESET", 0xffff);
    canvas.insert_text(POINT<>(err_row+=18, 5), "button to restart", 0xffff);
    canvas.insert_text(POINT<>(err_row+=18, 5), "system!", 0xffff);
    canvas.insert_text(POINT<>(err_row+=18, 5), "", 0xffff);
    canvas.insert_text(POINT<>(err_row+=18, 5), "Contact info:", 0xffff);
    canvas.insert_text(POINT<>(err_row+=18, 5), "> fb.com/ngxxfus", 0xffff);
    canvas.insert_text(POINT<>(err_row+=18, 5), "> msnp@outlook.com", 0xffff);
    canvas.insert_text(POINT<>(err_row+=18, 5), ".vn", 0xffff);
    canvas.show(true);
}

/// @brief custom isr service
void sw_isr_service(void){
    msg2ser("call\t", "sw_isr_service <mode ",  screen_mode, ">");
    if(screen_mode == enum_SCREEN_MODE::SETUP_WIFI_MODE)
        return;
    if(screen_mode == enum_SCREEN_MODE::NORMAL_MODE){
        run_show_envinfo = true;
        screen_mode = enum_SCREEN_MODE::SHOW_ENVINFO_MODE;
        msg2ser("\t", "screen_mode: ", screen_mode);
        return;
    };
    if(screen_mode == enum_SCREEN_MODE::SHOW_ENVINFO_MODE){
        run_show_envinfo = false;
        screen_mode = enum_SCREEN_MODE::NORMAL_MODE;
        msg2ser("\t", "screen_mode: ", screen_mode);
        return;
    }
}