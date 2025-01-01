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
    canvas.insert_text(POINT<>(18, 48), "- DEV -", 0x2124);
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
    controller::iled_blinky(10, 1000);
}

void get_and_show_image(uint16_t& i, bool auto_update = true, bool increase = true, bool show_before_update = true){
    msg2ser("call\t", "get_and_show_image");
    
    vector<String>  imgs_list = sdcard_list_dir(SD, "/imgs", 0);
    if(imgs_list.empty()) return;
    if(i >= imgs_list.size() ) i = 0;
    if(show_before_update)
        read_and_insert_565format_image(
            SD, 
            concatenate("/imgs/", imgs_list[i]), 
            POINT<>(0,0), 172, 220
        );
    if(auto_update){
        if(increase)
            i = (i < imgs_list.size()-1)?(i+1):0;
        else 
            i = (i > 0) ? (i-1):(imgs_list.size()-1);
    }
    if(!show_before_update)
        read_and_insert_565format_image(
            SD, 
            concatenate("/imgs/", imgs_list[i]), 
            POINT<>(0,0), 172, 220
        );
}

/// @brief slide show mode
void slideshow_mode(){
    msg2ser("call\t", "slideshow_mode");

    while(sw_value() == false) controller::iled_blinky(5);

    bool        selected = false;
    uint8_t     title0 = 2;
    uint16_t    selectedBox_W = 68, selectedBox_H = 31, btn0 = 190,
                img_pos = 0, sel = 0, prev_sel = 2;
    uint32_t    last_t = 0;
    DELAY_CTL   delay0(60000U);

    get_and_show_image(img_pos, false);

    while(0x1){

        if(sel == 0){
            /// for slideshow image control :v
            if( x_adc_value() < 30 ) {
                while(x_adc_value() < 30); 
                get_and_show_image(img_pos, true, true, false);
            }else 
                if ( x_adc_value() > 190 ) {
                    while(x_adc_value() > 190);
                    get_and_show_image(img_pos, false, false, false);
                }
        }else{
            /// for slideshow image control :v
            if( x_adc_value() < 30 ) {
                sel=(sel==1)?2:1;
                while(x_adc_value() < 30);
            }else 
                if ( x_adc_value() > 190 ) {
                    sel=(sel==1)?2:1;
                    while(x_adc_value() > 190);
                }
        }
        /// process sw_value() for selected box
        if(sw_value() == false){
            unsigned long start_t = millis();
            /// delay 100ms for stable
            delay(100);
            /// for long press?
            while ( sw_value() == false ){
                if(t_since(start_t) > hold_for_select){
                    selected = true;
                    break;
                }
            }
            /// mode route 0 
            if(sel > 0 && selected == true){
                /// clear selected flag
                selected = false; 
                /// hide screen 
                sel = 0;
                /// show image
                get_and_show_image(img_pos, false); 
                goto SHOW_CHANGED;
            }
            /// process corresponding mode :v 
            if( sel != 0){
                /// process corresponding mode :v 
                switch (sel){
                    case 2:
                        /// exit button
                        msg2ser("\t", "process ```exit``` button", "!");
                        screen_mode = enum_SCREEN_MODE::SHOW_ENVINFO_MODE; 
                        while(sw_value() == false) controller::iled_blinky(1);
                        return;
                    case 1:
                        /// reserve feature
                        msg2ser("\t", "process ```mode``` button", "!");
                        screen_mode = enum_SCREEN_MODE::RESERVED_FEATURE_MODE; 
                        while(sw_value() == false) controller::iled_blinky(1);
                        return;
                }
            }
            /// mode route 1
            if(sel == 0){
                sel = 1; goto SHOW_CHANGED;
            }
        }

        if(delay0.is_able_to_run(true))
            get_and_show_image(img_pos, true);

        /// re-draw image
        if(sel > 0 && sel != prev_sel) {
            canvas.refill(0xFFFF);
            /// show title
            canvas.insert_rectangle(POINT<>(title0-1, 1), 170, 35, 0x18c3, true, sensors_color_1);
            canvas.insert_text(POINT<>(title0+22, 50), "Slideshow", sensors_color_2);
            /// show image icon
            canvas.insert_rectangle(POINT<>(65, 33), 100, 66, 0x4208);
            canvas.insert_bitmap_image(POINT<>(65, 33), img_icon100x66, 100, 66, 0xb6df);
            /// show next/mode button
            show_2button_on_1line( btn0, "Mode", 25, "Setup", 105, sensors_color_4, sensors_color_6, sensors_color_5, sensors_color_7 );
            /// show selected box based on ```sel```
            switch (sel){
                case 1:
                    canvas.insert_rectangle(POINT<>(btn0-2, 13), selectedBox_W, selectedBox_H, sensors_color_21);
                    break;
                case 2:
                    canvas.insert_rectangle(POINT<>(btn0-2, 91), selectedBox_W, selectedBox_H, sensors_color_21);
                    break;
            }
            prev_sel = sel;
        }
        SHOW_CHANGED:
        canvas.show();
    }

}

/// @brief test screen color
void test_mode_screen(){
    msg2ser("call\t", "test_mode_screen");
    uint16_t color = 0;
    auto sdcard_list = sdcard_list_dir(SD, "/imgs", 0);
    for(auto p:sdcard_list) msg2ser("\t", p);
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
    
    // read_and_show_565format_image(SD, "/imgs/img0.bin", POINT<>(0,0), 172, 220);
    canvas.show(true);

    while(0x1)  controller::iled_blinky(1);
}

/// @brief custom isr service
void sw_isr_service(void){
    msg2ser("call\t", "sw_isr_service <mode ",  screen_mode, ">");
    if(screen_mode == enum_SCREEN_MODE::TEST_MODE_BLANK){
        screen_mode = enum_SCREEN_MODE::NORMAL_MODE;
    }
    if(screen_mode == enum_SCREEN_MODE::NORMAL_MODE){
        // sel = (sel < 2)?(sel+1):(0);
    }
}

