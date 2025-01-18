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

#if USB_SERIAL == true
    #include "serial_utils.h"
#endif

#if BASIC_IO == true
    #include "basic_io_utils.h"
#endif

#include "images.h"
#include "tft_utils.h"
#include "general_utils.h"


#if CONTROLLER == true
    #include "controller_utils.h"
#endif

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
DELAY_CTL       delay0(60000U);
DELAY_CTL       delay1(30000U);

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
        basic_io::led0_blinky(10000, 19, 253);
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
        basic_io::led0_blinky(10, 17, 329);
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

    basic_io::led0_blinky(2, 23,35);

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


void slideshow_menuconfig_mode(){
    call("slideshow_menuconfig_mode");
    #if TFT_SCREEN == true

        while(basic_io::btn1_val() == false) basic_io::led0_blinky(5);

        uint8_t     title0 = 2;
        uint16_t    selectedBox_W = 68, selectedBox_H = 31, btn0 = 190,
                    sel = 0, prev_sel = 2;
        const uint16_t  max_sel = 3;
        uint32_t        last_t = 0;

        while (0x1){

            /// UP
            if( sel == 0 && basic_io::btn3_val() == false ) {
                sel = (sel + 1) % max_sel;
                delay(50); while(basic_io::btn2_val() == false);
            }
            /// DOWN
            if( sel == 0 &&  basic_io::btn2_val() == false){ 
                sel = (sel == max_sel-1)?(0):(sel-1);
                delay(50); while(basic_io::btn2_val() == false);
            }
            /// OK
            if( sel != 0 &&   basic_io::btn1_val() == false){
                /// waiting for released from user
                delay(50); while(basic_io::btn2_val() == false);
            }
            /// exit
            if(basic_io::btn0_val() == false){
                delay(50); while(basic_io::btn2_val() == false);
                return;
            }

            /// clear canvas
            canvas.refill(0xFFFF);
            /// show title
            canvas.insert_rectangle(POINT<>(title0-1, 1), 170, 35, 0x18c3, true, sensors_color_1);
            canvas.insert_text(POINT<>(title0+22, 45), "menuconfig", sensors_color_2);
            /// show guide
            canvas.insert_text(POINT<>(70, 145),    "UP", 0xe73c);
            canvas.insert_text(POINT<>(70+30, 145), "DN", 0xe73c);
            canvas.insert_text(POINT<>(70+60, 145), "OK", 0xe73c);
            canvas.insert_text(POINT<>(70+90, 145), "Q*", 0xe73c);
            /// show option - slideshow duration
            canvas.insert_text(POINT<>(title0+65, 5), "Img duration:", 0x10a9);
            canvas.insert_text(POINT<>(title0+65, 105), String(delay0.get_interval()/1000), 0x19ec);
            /// show option - show temp,humid
            canvas.insert_text(POINT<>(title0+90, 5),  "Env info:", 0x10a9);
            canvas.insert_text(POINT<>(title0+90, 105), String(delay1.forced_stop()?"N":"Y"), 0x19ec);
            canvas.insert_text(POINT<>(title0+115, 5), "Env interval:", 0x10a9);
            canvas.insert_text(POINT<>(title0+115, 105), String(delay1.get_interval()/1000), 0x19ec);
            

            SHOW_CHANGED:
            canvas.show();
        }
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

        get_and_show_image(img_pos, false);

        while(0x1){
            SLIDESHOW:
            
            // basic_io::toggle_led1_state();
            // log2ser(
            //     "btn0: ", basic_io::btn0_recent_pressed,
            //     " btn1: ", basic_io::btn1_recent_pressed,
            //     " btn2: ", basic_io::btn2_recent_pressed,
            //     " btn3: ", basic_io::btn2_recent_pressed
            // );

            /// @brief for slideshow image control :v
            if( basic_io::btn3_is_recent_pressed() == true ){
                get_and_show_image(img_pos, true, true, false);
            }
            if( basic_io::btn2_is_recent_pressed() == true ){ 
                get_and_show_image(img_pos, true, false, false);
            }

            /// @brief enter selection
            if( basic_io::btn1_is_recent_pressed() == true ){
                if(sel == 1){
                    slideshow_menuconfig_mode();
                    /// re-draw img
                    get_and_show_image(img_pos, false); 
                    goto SHOW_CHANGED;
                }
                if(sel == 2){
                    screen_mode = enum_SCREEN_MODE::NORMAL_MODE;
                    break;
                }
            }

            /// @brief for bounding box, other controls
            if( basic_io::btn0_is_recent_pressed() == true ){
                sel = (sel+1) % 3;
                if(sel == 0){
                    prev_sel = sel;
                    get_and_show_image(img_pos, false); 
                    goto SHOW_CHANGED;
                }
            }

            /// periody update image
            if( delay0.time_to_run(true) == true )
                get_and_show_image(img_pos, true);

            /// update temp&humid
            if( delay1.time_to_run() == true){
                basic_io::led0_blinky(1, 5, 5);
                canvas.insert_rectangle(POINT<>(174, 2), 124, 43, 0x18c3, true, 0x18c3);
                /// insert humid
                canvas.insert_text(POINT<>(190, 5), "Humid: ", 0x94b2);
                canvas.insert_text(POINT<>(190, 70), String(sensors::read_humid()), 0x94b2);
                /// insert temp
                canvas.insert_text(POINT<>(210, 5), "Temp: ", 0x94b2);
                canvas.insert_text(POINT<>(210, 70), String(sensors::read_temp()), 0x94b2);
                basic_io::led0_val(LOW);
            }

            /// draw guide
            if(sel > 0 && sel != prev_sel) {
                canvas.refill(0xFFFF);
                /// show image 
                get_and_show_image(img_pos, false); 
                /// show title
                canvas.insert_rectangle(POINT<>(title0-1, 2), 168, 35, 0x18c3, true, sensors_color_1);
                canvas.insert_text(POINT<>(title0+22, 50), "Slideshow", sensors_color_2);
                /// show guide
                canvas.insert_text(POINT<>(65, 145),    "N", 0xad55);
                canvas.insert_text(POINT<>(65+30, 145), "P", 0xad55);
                canvas.insert_text(POINT<>(65+60, 145), "O", 0xad55);
                canvas.insert_text(POINT<>(65+90, 145), "M", 0xad55);
                /// show next/mode button
                show_2button_on_1line( btn0, "Setup", 25, "Exit", 110, sensors_color_4, sensors_color_6, sensors_color_5, sensors_color_7 );
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

/// >>>>>>>>>>>>>>>>>>>> TEST FN DEFINITIONS >>>>>>>>>>>>>>>>>>>>>>>>
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