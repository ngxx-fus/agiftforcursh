#include <Arduino.h>
#include <algorithm>
#include <limits>
using namespace std;

#define  elif else if

/// >>>>>>>>>>>>>>>>>>>>> PIN DEFINE >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

#define DHT_TYPE                DHT11
#define DHT_PIN                 16

#define TFT_RST_PIN (int8_t)    2
#define TFT_RS_PIN  (int8_t)    4
#define SPI_BUS_TYPE            HSPI

#define SDCARD_SPI_CS_PIN       5

#define BTN0_PIN                32 
#define BTN1_PIN                34
#define BTN2_PIN                35         
#define BTN3_PIN                17

#define LED0_PIN                26
#define LED1_PIN                27
 
/// >>>>>>>>>>>>>>>>>>>>> HEADER INCLUDES >>>>>>>>>>>>>>>>>>>>>>>>>>
#include "images.h"

#if USB_SERIAL == true
    #include "serial_utils.h"
#endif

#if BASIC_IO == true
    #include "basic_io_utils.h"
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
vector<String>  imgs_list;


/// @brief show error screen
void error_mode(){
    #if TFT_SCREEN == true
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
    #endif
    #if BASIC_IO == true
        basic_io::led0_val(1);
        basic_io::led1_blinky(10000, 19, 253);
    #endif
    #if CONTROLLER == true
        while(0x1)  controller::iled_blinky(1);
    #endif
}

/// @brief show notification screen for reserved feature
void reserved_feature_mode(){
    #if TFT_SCREEN == true
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
    #endif
    #if BASIC_IO == true
        basic_io::led0_val(1);
        basic_io::led1_blinky(10000, 17, 329);
    #endif
    #if CONTROLLER == true
        controller::iled_blinky(10, 1000);
    #endif
}

uint32_t update_imgs_list(){
    #if SDCARD_RW == true
        call("update_imgs_list");
        imgs_list = sdcard_list_dir(SD, "/imgs", 0);
    #endif
    return imgs_list.size();
}

void get_and_show_image(
    uint16_t& i, bool auto_update = true, 
    bool increase = true, bool show_before_update = true
){
    call( "get_and_show_image");

    basic_io::led1_blinky(2, 23,35);

    #if SDCARD_RW == true
        if(imgs_list.empty()) return;
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
    #endif
}

/// @brief slide show mode
void slideshow_mode(){
    /// [3] : next image
    /// [2] : prev image
    /// [1] : next mode (aka OKE)
    /// [0] : menu mode 

    call( "slideshow_mode");

    #if TFT_SCREEN == true
        update_imgs_list();

        while(basic_io::btn1_val() == false) basic_io::led0_blinky(5);

        bool        selected = false;
        uint8_t     title0 = 2;
        uint16_t    selectedBox_W = 68, selectedBox_H = 31, btn0 = 190,
                    img_pos = 0, sel = 0, prev_sel = 2;
        uint32_t    last_t = 0;
        DELAY_CTL   delay0(60000U);

        get_and_show_image(img_pos, false);

        while(0x1){
            /// for slideshow image control :v
            if( sel == 0 && basic_io::btn3_val() == false ) {
                get_and_show_image(img_pos, true, true, false);
                delay(50);
                while(basic_io::btn3_val() == false); 
            }
            if( sel == 0 &&  basic_io::btn2_val() == false){ 
                get_and_show_image(img_pos, true, false, false);
                delay(50);
                while(basic_io::btn2_val() == false);
            }
            /// enter selection
            /// for bounding box, other controls
            if( sel != 0 &&   basic_io::btn1_val() == false){
                /// waiting for released from user
                delay(50);
                while( basic_io::btn1_val() == false);
                if(sel == 1){
                    screen_mode = enum_SCREEN_MODE::RESERVED_FEATURE_MODE;
                    while(0x1) reserved_feature_mode();
                    break;
                }
                if(sel == 2){
                    screen_mode = enum_SCREEN_MODE::NORMAL_MODE;
                    break;
                }
                
            }
            /// for bounding box, other controls
            if(basic_io::btn0_val() == false){
                /// waiting for released from user
                delay(50);
                while( basic_io::btn0_val() == false);
                sel = (sel+1) % 3;
                if(sel == 0){
                    /// show image
                    prev_sel = sel;
                    get_and_show_image(img_pos, false); 
                    goto SHOW_CHANGED;
                }
                
            }

            /// periody update image
            if( delay0.time_to_run(true) == true )
                get_and_show_image(img_pos, true);

            /// re-draw image
            if(sel > 0 && sel != prev_sel) {
                canvas.refill(0xFFFF);
                /// show image icon
                get_and_show_image(img_pos, false); 
                /// show title
                canvas.insert_rectangle(POINT<>(title0-1, 1), 170, 35, 0x18c3, true, sensors_color_1);
                canvas.insert_text(POINT<>(title0+22, 50), "Slideshow", sensors_color_2);
                /// show next/mode button
                show_2button_on_1line( btn0, "Setup", 25, "Exit", 105, sensors_color_4, sensors_color_6, sensors_color_5, sensors_color_7 );
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
    #endif
}

/// @brief test screen color
void test_mode_screen(){
    call( "test_mode_screen");
    #if TFT_SCREEN == true
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
    #endif
}

/// @brief blank screen to test
void test_mode_blank(){ 
    #if TFT_SCREEN == true
        call( "test_mode_blank");
        canvas.refill(0xFFFF);
        uint16_t err_row = 18;
        canvas.insert_text(POINT<>(18, 48), "TEST SYS", 0x0);
        #if SDCARD_RW == true
            auto sdcard_list = sdcard_list_dir(SD, "/", 0);
            if(sdcard_list.empty() == false){
                rept(int16_t, i, 0, sdcard_list.size()-1){
                    if(i > 5) break; /// cannot show more than 6 lines
                    canvas.insert_text(POINT<>(err_row+=18, 2), sdcard_list[i], 0x0);
                }
            }
        #endif
        
        // read_and_show_565format_image(SD, "/imgs/img0.bin", POINT<>(0,0), 172, 220);
        canvas.show(true);
        #if CONTROLLER == true
            while(0x1)  controller::iled_blinky(1);
        #endif
    #endif
}

/// @brief custom isr service
void sw_isr_service(void){
    call( "sw_isr_service <mode ",  screen_mode, ">");
    if(screen_mode == enum_SCREEN_MODE::TEST_MODE_BLANK){
        screen_mode = enum_SCREEN_MODE::NORMAL_MODE;
    }
    if(screen_mode == enum_SCREEN_MODE::NORMAL_MODE){
        // sel = (sel < 2)?(sel+1):(0);
    }
}

#if HARDWARE_TEST == true
    void btn0_isr(){
        msg2ser("log2ser\t", "btn0 is pressed!");
        delay(50);
        // digitalWrite(LED0_PIN, !digitalRead(LED0_PIN));
    }
    void btn1_isr(){
        msg2ser("log2ser\t", "btn1 is pressed!");
        delay(50);
        // digitalWrite(LED0_PIN, !digitalRead(LED0_PIN));
    }
    void btn2_isr(){
        msg2ser("log2ser\t", "btn2 is pressed!");
        delay(50);
        // digitalWrite(LED1_PIN, !digitalRead(LED1_PIN));
    }
    void btn3_isr(){
        msg2ser("log2ser\t", "btn3 is pressed!");
        delay(50);
        // digitalWrite(LED1_PIN, !digitalRead(LED1_PIN));
    }
#endif