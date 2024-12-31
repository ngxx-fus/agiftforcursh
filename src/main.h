#include <Arduino.h>
#include <algorithm>
#include <limits>

using namespace std;

#define  elif else if

/// >>>>>>>>>>>>>>>>>>>>> PIN DEFINE >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

#define VR_X_PIN                35
#define VR_Y_PIN                34
#define SW_PIN                  17

#define DHT_TYPE                DHT11
#define DHT_PIN                 16

#define TFT_RST_PIN (int8_t)    2
#define TFT_RS_PIN  (int8_t)    4
#define SPI_BUS_TYPE            HSPI

#define SDCARD_SPI_CS_PIN       5

/// >>>>>>>>>>>>>>>>>>>>> HEADER INCLUDES >>>>>>>>>>>>>>>>>>>>>>>>>>
#include "images.h"

#if USB_SERIAL == true
    #include "serial_utils.h"
#endif

#if CONTROLLER == true
    #include "controller_utils.h"
#endif

#include "tft_utils.h"
#include "general_utils.h"

#if WIFI_CONNECTION == true
    #include "wifi_utils.h"
#endif

#if SENSORS == true
    #include "sensor_utils.h"
#endif

#if FIREBASE_RTDB == true
    #include "firebase_utils.h"
#endif

#if SDCARD_RW == true
    #include "sdcard_utils.h"
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
    while(0x1)  controller::iled_blinky(1);
}

/// @brief show notification screen for reserved feature
void reserved_feature_mode(){
    canvas.refill(0x7fd4);
    uint16_t err_row = 18;
    canvas.insert_text(POINT<>(18, 52), "SYSTEM", 0x2124);
    canvas.insert_text(POINT<>(err_row+=18, 5), "Hello!", 0x2124);
    canvas.insert_text(POINT<>(err_row+=18, 5), "This is NOT an", 0x2124);
    canvas.insert_text(POINT<>(err_row+=18, 5), "ERROR. This is a", 0x2124);
    canvas.insert_text(POINT<>(err_row+=18, 5), "RESERVED feature!", 0x2124);
    canvas.insert_text(POINT<>(err_row+=18, 5), "Press RESET to", 0x2124);
    canvas.insert_text(POINT<>(err_row+=18, 5), "restart :>", 0x2124);
    canvas.insert_text(POINT<>(err_row+=18, 5), "Contact info:", 0x2124);
    canvas.insert_text(POINT<>(err_row+=18, 5), "> fb.com/ngxxfus", 0x2124);
    canvas.insert_text(POINT<>(err_row+=18, 5), "> msnp@outlook.com", 0x2124);
    canvas.insert_text(POINT<>(err_row+=18, 5), ".vn", 0x2124);
    canvas.show(true);
    while(0x1)  controller::iled_blinky(1);
}

/// @brief slide show mode
void slideshow_mode(){
    msg2ser("call\t", "slideshow_mode");

    uint8_t title0 = 2;
    static uint16_t curr_pos = 0;
    uint32_t last_t = 0;
    uint16_t sel = 0, selectedBox_W = 68, selectedBox_H = 31, btn0 = 190;
    bool selected = false;

    while(0x1){


        /// for Bar spacing :v
        if( x_adc_value() < 30 ) {
            msg2ser("\t", "Image: +1 next");
            curr_pos = (curr_pos < 4)?(curr_pos+1):(0);
        }else 
            if ( x_adc_value() > 190 ) {
                msg2ser("\t", "Image: +1 previos");
                curr_pos = (curr_pos > 0)?(curr_pos-1):(0);
            }

        /// clear screen
        canvas.refill(sensors_color_0);
        
        /// show image
        canvas.insert_565color_image(POINT<>(0,0), slideshow_img, 172, 220);

        /// show title
        canvas.insert_rectangle(POINT<>(title0-1, 1), 170, 35, 0x18c3, true, sensors_color_1);
        canvas.insert_text(POINT<>(title0+22, 50), "Slideshow", sensors_color_2);
        /// show next mode button
        show_2button_on_1line( btn0, "Mode", 25, "Setup", 105, sensors_color_4, sensors_color_6, sensors_color_5, sensors_color_7 );

        /// process sw_value() for selected box
        if(sw_value() == false){
            msg2ser("\t", "sw_value():", "is pressed");
            unsigned long start_t = millis();
            /// delay 100ms for stable
            delay(100);
            while ( sw_value() == false ){
                if(t_since(start_t) > hold_for_select){
                    selected = true;
                    break;
                }
            }
            msg2ser("\t", "selected: ", selected);
            
            if(selected){
                /// clear selected flag
                selected = false;
                /// process corresponding mode :v 
                switch (sel){
                    case 2:
                        msg2ser("\t", "process ```Mode``` button", "!");
                        /// exit button
                        screen_mode = enum_SCREEN_MODE::SHOW_ENVINFO_MODE;
                        return;
                    case 1:
                        msg2ser("\t", "process ```setup``` button", "!");
                        /// reserve feature
                        screen_mode = enum_SCREEN_MODE::RESERVED_FEATURE_MODE;
                        return;
                }
            }else{
                /// increase sel
                sel = (sel < 2)?(sel+1):(0);
            }
        }

        /// insert selected box based on ```sel```
        switch (sel){
            /// case 0: just do nothin' and hide selected box
            case 1:
                /// show selected box
                canvas.insert_rectangle(POINT<>(btn0-2, 13), selectedBox_W, selectedBox_H, sensors_color_21);
                break;
            case 2:
                /// show selected box
                canvas.insert_rectangle(POINT<>(btn0-2, 91), selectedBox_W, selectedBox_H, sensors_color_21);
                break;
        }

        /// show changed
        canvas.show();
    }

}

/// @brief test screen color
void test_mode_screen(){
    msg2ser("call\t", "test_mode_screen");
    uint16_t color = 0;
    canvas.insert_text(POINT<>(100, 1), "TEST SCREEN", 0x2124);
    canvas.show();
    rept(uint16_t, i, 0, 218){
        rept(uint16_t, j, 0, 170){
            tft.drawPixel(j, i, color++);
        }
    }
    rept(uint16_t, i, 0, 218){
        rept(uint16_t, j, 0, 170){
            tft.drawPixel(j, i, color++);
        }
    }
}

/// @brief blank screen to test
void test_mode_blank(){ 
    msg2ser("call\t", "test_mode_blank");
    canvas.refill(0xFFFF);
    uint16_t err_row = 18;
    canvas.insert_text(POINT<>(18, 48), "TEST SYS", 0x0);
    
    auto sdcard_list = sdcard_list_dir(SD, "/", 0);

    if(sdcard_list.empty() == false){
        rept(int16_t, i, 0, sdcard_list.size()-1){
            if(i > 5) break; /// cannot show more than 6 lines
            canvas.insert_text(POINT<>(err_row+=18, 2), sdcard_list[i], 0x0);
        }
    }
    canvas.show(true);

    readFile(SD, "/imgs/img0.txt");

    while(0x1)  controller::iled_blinky(1);
}

/// @brief custom isr service
void sw_isr_service(void){
    msg2ser("call\t", "sw_isr_service <mode ",  screen_mode, ">");
    if(screen_mode == enum_SCREEN_MODE::TEST_MODE_SCREEN){
        screen_mode = enum_SCREEN_MODE::NORMAL_MODE;
    }
}

