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

/// >>>>>>>>>>>>>>>>>>>>> COLOR DEFINITIONS >>>>>>>>>>>>>>>>>>>>>>>>

#define                     GUIDE_COLOR                 0xef7d
#define                     SELECTED_BOX                0x9e1d
 
/// >>>>>>>>>>>>>>>>>>>>> HEADER INCLUDES >>>>>>>>>>>>>>>>>>>>>>>>>>
#define  elif else if

#include <Arduino.h>

#if SENSORS == true
    #include "DHT.h"
#endif

#if WIFI_CONNECTION == true
    #include "WiFi.h"
    #include "FirebaseESP32.h"
#endif

#if USB_SERIAL == true
    #include "serial_utils.h"
#endif

#if BASIC_IO == true
    #include "basic_io_utils.h"
#endif

#if FIREBASE_RTDB == true
    #include "firebase_utils.h"
#endif

#include "images.h"
#include "tft_utils.h"
#include "general_utils.h"

#if WIFI_CONNECTION == true
    #include "wifi_utils.h"
#endif

#if CONTROLLER == true
    #include "controller_utils.h"
#endif

#if SENSORS == true
    #include "sensor_utils.h"
#endif


#if SDCARD_RW == true
    #include "sdcard_utils.h"
#endif

/// >>>>>>>>>>>>>>>>>>>> OTHERS DEFINITIONS >>>>>>>>>>>>1>>>>>>>>>>>>

#ifndef BTN_PRESSED
    #define BTN_PRESSED
    static uint8_t btn_pressed = 0x0;
#endif

vector<String>              imgs_list;
DELAY_CTL                   delay0(60000U);
DELAY_CTL                   delay1(30000U);
float                       humid = 0.0, temp = 0.0;
static bool                 show_env_info = true;
static bool                 turn_on_bedroom_light = true;

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
        #if LOG == true
            call("update_imgs_list");
        #endif
        imgs_list = sdcard_list_dir(SD, "/imgs", 0);
    #endif
    return imgs_list.size();
}

void show_humid_temp_box(POINT<> pos, uint16_t background_color = 0x0, uint16_t text_color = 0xFFFF){
    /// insert background
    canvas.insert_rectangle(pos, 120, 43, background_color, true, background_color);
    /// insert humid/
    canvas.insert_text(POINT<>(pos.X()+16, pos.Y()+5), "Humid: ", text_color);
    canvas.insert_text(POINT<>(pos.X()+16, pos.Y()+70), String(humid), text_color);
    /// insert temp
    canvas.insert_text(POINT<>(pos.X()+36, pos.Y()+5), "Temp: ", text_color);
    canvas.insert_text(POINT<>(pos.X()+36, pos.Y()+70), String(temp), text_color);
}

void get_and_show_image(
    uint16_t& i, bool auto_update = true, 
    bool increase = true, bool show_before_update = true,
    bool temporary_hide_humid_temp_box = false
){
    call( "get_and_show_image");

    if(!sdcard_is_available) return;
    
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
    if(!temporary_hide_humid_temp_box && show_env_info){
        show_humid_temp_box(POINT<>(174, 2), 0x18c3, 0x94b2);
    }
}

static void slideshow_btn3_isr(){
    #if LOG == true
        call( "slideshow_btn3_isr");
    #endif
    btn_pressed |= 0x1;
}
static void slideshow_btn2_isr(){
    #if LOG == true
        call( "slideshow_btn2_isr");
    #endif
    btn_pressed |= 0x2;
}
static void slideshow_btn1_isr(){
    #if LOG == true
        call( "slideshow_btn1_isr");
    #endif
    btn_pressed |= 0x4;
}
static void slideshow_btn0_isr(){
    #if LOG == true
        call( "slideshow_btn0_isr");
    #endif
    btn_pressed |= 0x8;
}

void slideshow_menuconfig_mode(){
    /// [3] UP 
    /// [2] DOWN
    /// [1] CH
    /// [0] QUIT

    #if LOG == true
        call("slideshow_menuconfig_mode");
    #endif
    #if TFT_SCREEN == true
        btn_pressed = 0x0;

        while(basic_io::btn1_val() == false) basic_io::led0_blinky(5);

        uint8_t     title0 = 2;
        uint16_t    btn_pressedBox_W = 68, btn_pressedBox_H = 31, btn0 = 190,
                    sel = 0, prev_sel = 2, 
                    x_option0 = title0+65, x_option1 = title0+90, 
                    x_option2 = title0+115, x_option3 = title0+140;
        const uint16_t  max_sel = 4;
        uint32_t        last_t = 0;

        while (0x1){

            /// UP
            if( btn_pressed & 0x1 ){
                switch (sel){
                    case 0:
                        delay0.set_interval(
                            (delay0.get_interval())%600000U + 10000
                        );
                        delay(100);
                        while(!basic_io::btn1_val()){
                            delay0.set_interval(
                                (delay0.get_interval())%600000U + 10000
                            );
                            delay(200);
                        }
                        break;
                    case 1:
                        delay(300);
                        show_env_info = ! show_env_info;
                        break;
                    case 2:
                        delay1.set_interval(
                            (delay1.get_interval())%600000U + 10000
                        );
                        delay(100);
                        while(!basic_io::btn1_val()){
                            delay1.set_interval(
                                (delay1.get_interval())%600000U + 10000
                            );
                            delay(200);
                        }
                        break;
                    case 3:
                        delay(300);
                        turn_on_bedroom_light = ! turn_on_bedroom_light;
                        break;
                }
                btn_pressed = btn_pressed & 0xFE;
            }
            /// DOWN
            if( btn_pressed & 0x2){ 
                switch (sel){
                    case 0:
                        delay0.set_interval(
                            (delay0.get_interval())?(delay0.get_interval()-10000):(600000U)
                        );
                        delay(100);
                        while(!basic_io::btn1_val()){
                            delay0.set_interval(
                                (delay0.get_interval())?(delay0.get_interval()-10000):(600000U)
                            );
                            delay(200);
                        }
                        break;
                    case 1:
                        delay(300);
                        show_env_info = ! show_env_info;
                        break;
                    case 2:
                        delay1.set_interval(
                            (delay1.get_interval())?(delay1.get_interval()-10000):(600000U)
                        );
                        delay(100);
                        while(!basic_io::btn1_val()){
                            delay1.set_interval(
                                (delay1.get_interval())?(delay1.get_interval()-10000):(600000U)
                            );
                            delay(200);
                        }
                        break;
                    case 3:
                        delay(300);
                        turn_on_bedroom_light = ! turn_on_bedroom_light;
                        break;
                }
                btn_pressed = btn_pressed & 0xFD;
            }
            /// CH
            if( btn_pressed & 0x4 ){
                sel = (sel+1)%max_sel;
                delay(200);
                btn_pressed = btn_pressed & 0xFB;
            }
            /// OKE
            if( btn_pressed & 0x8 ){
                btn_pressed = btn_pressed & 0xFB;
                goto MENUCONFIG_SLIDESHOW_SAFE_EXIT;
            }

            /// clear canvas
            canvas.refill(0xFFFF);
            /// show title
            canvas.insert_rectangle(POINT<>(title0-1, 1), 170, 35, 0x18c3, true, sensors_color_1);
            canvas.insert_text(POINT<>(title0+22, 45), "menuconfig", sensors_color_2);
            /// show guide
            canvas.insert_text(POINT<>(85, 145),    "UP", GUIDE_COLOR);
            canvas.insert_text(POINT<>(85+30, 145), "DN", GUIDE_COLOR);
            canvas.insert_text(POINT<>(85+60, 145), "CH", GUIDE_COLOR);
            canvas.insert_text(POINT<>(85+90, 145), "OK", GUIDE_COLOR);
            /// show option - slideshow duration
            canvas.insert_text(POINT<>(x_option0, 5), "Img duration:", 0x10a9);
            canvas.insert_text(POINT<>(x_option0, 110), String(delay0.get_interval()/1000), 0x19ec);
            /// show option - show temp,humid
            canvas.insert_text(POINT<>(x_option1, 5),  "Env info:", 0x10a9);
            canvas.insert_text(POINT<>(x_option1, 110), String(show_env_info?"Y":"N"), 0x19ec);
            canvas.insert_text(POINT<>(x_option2, 5), "Env interval:", 0x10a9);
            canvas.insert_text(POINT<>(x_option2, 110), String(delay1.get_interval()/1000), 0x19ec);
            /// show option - show bedroom led light
            canvas.insert_text(POINT<>(x_option3, 5), "Bedroom Light:", 0x10a9);
            canvas.insert_text(POINT<>(x_option3, 110), String(turn_on_bedroom_light?"Y":"N"), 0x19ec);
            
            /// show current selection
            switch(sel){
                case 0:
                    canvas.insert_rectangle(POINT<>(x_option0-17, 2), 138, 25, SELECTED_BOX);
                    break;
                case 1:
                    canvas.insert_rectangle(POINT<>(x_option1-17, 2), 138, 25, SELECTED_BOX);
                    break;
                case 2:
                    canvas.insert_rectangle(POINT<>(x_option2-17, 2), 138, 25, SELECTED_BOX);
                    break;
                case 3:
                    canvas.insert_rectangle(POINT<>(x_option3-17, 2), 138, 25, SELECTED_BOX);
                    break;
            }

            SHOW_CHANGED:
            canvas.show();
        }
    #endif
    return;
    MENUCONFIG_SLIDESHOW_SAFE_EXIT:
    btn_pressed = 0x0;
    return;
}

/// @brief slide show mode
void slideshow_mode(){
    /// [3] : next image
    /// [2] : prev image
    /// [1] : next mode (aka OKE)
    /// [0] : menu mode 

    #if LOG == true
        call( "slideshow_mode");
    #endif
    
    basic_io::btn3_attach_interrupt(slideshow_btn3_isr);
    basic_io::btn2_attach_interrupt(slideshow_btn2_isr);
    basic_io::btn1_attach_interrupt(slideshow_btn1_isr);
    basic_io::btn0_attach_interrupt(slideshow_btn0_isr);

    #if TFT_SCREEN == true
        update_imgs_list();

        while(basic_io::btn1_val() == false) basic_io::led0_blinky(5);

        uint8_t     title0 = 2;
        uint16_t    btn_pressedBox_W = 68, btn_pressedBox_H = 31, btn0 = 190,
                    img_pos = 0, sel = 0, prev_sel = 2;

        get_and_show_image(img_pos, false);

        while(0x1){
            SLIDESHOW:
            
            /// set bedroom led light
            #if BASIC_IO == true
                basic_io::led1_state_value(turn_on_bedroom_light);
            #endif

            /// @brief for slideshow image control :v
            /// @brief -> next
            if( btn_pressed & 0x1 ){
                btn_pressed = btn_pressed & 0xFE;
                get_and_show_image(img_pos, true, true, false);
                goto SHOW_CHANGED;
            }

            /// @brief -> previous
            if( btn_pressed & 0x2 ){ 
                btn_pressed = btn_pressed & 0xFD;
                get_and_show_image(img_pos, true, false, false);
                goto SHOW_CHANGED;
            }

            /// @brief enter selection
            if( btn_pressed & 0x4 ){
                btn_pressed = btn_pressed & 0xFB;
                if(sel == 1){
                    /// goto menuconfig mode
                    slideshow_menuconfig_mode();
                    /// re-draw img
                    sel = 0; prev_sel = sel-1;
                    get_and_show_image(img_pos, false); 
                    goto SHOW_CHANGED;
                }
                if(sel == 2){
                    screen_mode = enum_SCREEN_MODE::NORMAL_MODE;
                    break;
                }
            }

            /// @brief for bounding box, other controls
            if( btn_pressed & 0x8 ){
                btn_pressed = btn_pressed & 0xF7;
                sel = (sel+1) % 3;
                if(sel == 0){
                    /// show image
                    get_and_show_image(img_pos, false); 
                    goto SHOW_CHANGED;
                }
            }

            /// periody update env info
            if(delay1.time_to_run()){
                humid = sensors::read_humid(0);
                temp = sensors::read_temp(0);
            }

            /// periody update image
            if( delay0.time_to_run(true)){
                SHOW_IMAGE:
                get_and_show_image(img_pos, true);
                goto SHOW_CHANGED;
            }
            

            /// draw guide
            if(sel > 0 && sel != prev_sel) {
                canvas.refill(0xFFFF);
                /// show image 
                get_and_show_image(img_pos, false, true, true, true); 
                /// show title
                canvas.insert_rectangle(POINT<>(title0-1, 2), 168, 35, 0x18c3, true, sensors_color_1);
                canvas.insert_text(POINT<>(title0+22, 50), "Slideshow", sensors_color_2);
                /// show guide
                canvas.insert_text(POINT<>(65, 145),    "N", GUIDE_COLOR);
                canvas.insert_text(POINT<>(65+30, 145), "P", GUIDE_COLOR);
                canvas.insert_text(POINT<>(65+60, 145), "O", GUIDE_COLOR);
                canvas.insert_text(POINT<>(65+90, 145), "M", GUIDE_COLOR);
                /// show next/mode button
                show_2button_on_1line( btn0, "Setup", 25, "Exit", 110, sensors_color_4, sensors_color_6, sensors_color_5, sensors_color_7 );
                /// show btn_pressed box based on ```sel```
                switch (sel){
                    case 1:
                        canvas.insert_rectangle(POINT<>(btn0-2, 13), btn_pressedBox_W, btn_pressedBox_H, sensors_color_21);
                        break;
                    case 2:
                        canvas.insert_rectangle(POINT<>(btn0-2, 91), btn_pressedBox_W, btn_pressedBox_H, sensors_color_21);
                        break;
                }
                prev_sel = sel;                                                       
            }
            SHOW_CHANGED:
            canvas.show();
        }
    #endif
    SLIDESHOW_SAFE_EXIT:
    btn_pressed = 0x0;
    basic_io::btn3_detach_interrupt();
    basic_io::btn2_detach_interrupt();
    basic_io::btn1_detach_interrupt();
    basic_io::btn0_detach_interrupt();
    return;
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