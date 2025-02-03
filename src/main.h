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
#define LOCAL_CONFIG            false
#define SKIP_WIFI_SETUP         true
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
#define BTN4_PIN                33

#define LED0_PIN                26
#define LED1_PIN                27

/// >>>>>>>>>>>>>>>>>>>>> COLOR DEFINITIONS >>>>>>>>>>>>>>>>>>>>>>>>

#define                     GUIDE_COLOR                 0xef7d
#define                     SELECTED_BOX                0x9e1d
 
/// >>>>>>>>>>>>>>>>>>>>> HEADER INCLUDES >>>>>>>>>>>>>>>>>>>>>>>>>>
#define  elif else if

#include <Arduino.h>

#if WIFI_CONNECTION == true
    #include "WiFi.h"
#endif

#if FIREBASE_RTDB == true
    #include "FirebaseESP32.h"
#endif

#if USB_SERIAL == true
    #include "serial_utils.h"
#endif

#if BASIC_IO == true
    #include "basic_io_utils.h"
#endif

#include "images.h"
#include "tft_utils.h"
#include "general_utils.h"

#if WIFI_CONNECTION == true
    #include "wifi_utils.h"
#endif

#if SENSORS == true
    #include "sensor_utils.h"
#endif

#if SDCARD_RW == true
    #include "sdcard_utils.h"
#endif

#if LOCAL_CONFIG == true
    #include "config_utils.h"
#endif

/// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
/// >>>>>                                                      >>>>>
/// >>>>>                                                      >>>>>
/// >>>>>                                                      >>>>>
/// >>>>>                                                      >>>>>
/// >>>>>>>>>>>>>>>>>>>> OTHERS DEFINITIONS >>>>>>>>>>>>>>>>>>>>>>>>

#ifndef BTN_PRESSED
    #define BTN_PRESSED
    static uint8_t btn_pressed = 0x0;
#endif



/// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
/// >>>>>                                                      >>>>>
/// >>>>>                                                      >>>>>
/// >>>>>                                                      >>>>>
/// >>>>>                                                      >>>>>
/// >>>>>>>>>>>>>>>>>>>>> SLIDESHOW MODE >>>>>>>>>>>>>>>>>>>>>>>>>>>
static uint8_t              light_level = 100U;
static bool                 show_env_info = true;
static bool                 turn_on_bedroom_light = true;
static DELAY_CTL            delay0(60000U);
static DELAY_CTL            delay1(30000U);
static float                humid = 0.0, temp = 0.0;
static POINT<uint16_t>                env_box_pos(174, 2);

void get_and_show_image(
    uint16_t& i, bool auto_update = true, 
    bool increase = true, bool show_before_update = true,
    bool temporary_hide_humid_temp_box = false
){
    call( "get_and_show_image");
    basic_io::led0_blinky(2, 23,35);

    #if SDCARD_RW == true

        if(!sdcard_imgs::is_available){
            sdcard_init();
            if(!sdcard_imgs::is_available){
                canvas.refill(0xFFFF);
                single_text_line(1, "[SDCard]");
                single_text_line(2, "un-available!");
                goto GET_AND_SHOW_IMAGE_SAFE_EXIT;
            }else{
                sdcard_imgs::list();
            }
        }
        if(sdcard_imgs::img_list.empty()){
            canvas.refill(0xFFFF);
            single_text_line(1, "[Images]");
            single_text_line(2, "No img found!");
            goto GET_AND_SHOW_IMAGE_SAFE_EXIT;
        }

        if(show_before_update){
            sdcard_imgs::cache_and_insert(canvas, i);
        }

        if(auto_update){
            /// update
            if(increase)
                i = (i < sdcard_imgs::img_list.size()-1)?(i+1):0;
            else 
                i = (i > 0) ? (i-1):(sdcard_imgs::img_list.size()-1);
        }

        if(!show_before_update){
            sdcard_imgs::cache_and_insert(canvas, i);
        }
    #endif
    GET_AND_SHOW_IMAGE_SAFE_EXIT:
    if(!temporary_hide_humid_temp_box && show_env_info){
        show_humid_temp_box(env_box_pos, humid, temp, 0x18c3, 0x94b2);
    }
}

static void slideshow_btn4_isr(){
    if(millis() - basic_io::btn4_last_pressed < 200) return;
    #if LOG == true
        call( "slideshow_btn4_isr");
    #endif
    basic_io::btn4_last_pressed = millis();
    btn_pressed |= basic_io::btn4_bmask;
}

static void slideshow_btn3_isr(){
    if(millis() - basic_io::btn3_last_pressed < 200) return;
    #if LOG == true
        call( "slideshow_btn3_isr");
    #endif
    basic_io::btn3_last_pressed = millis();
    btn_pressed |= basic_io::btn3_bmask;
}

static void slideshow_btn2_isr(){
    if(millis() - basic_io::btn2_last_pressed < 200) return;
    #if LOG == true
        call( "slideshow_btn2_isr");
    #endif
    basic_io::btn2_last_pressed = millis();
    btn_pressed |= basic_io::btn2_bmask;
}

static void slideshow_btn1_isr(){
    if(millis() - basic_io::btn1_last_pressed < 200) return;
    #if LOG == true
        call( "slideshow_btn1_isr");
    #endif
    basic_io::btn1_last_pressed = millis();
    btn_pressed |= basic_io::btn1_bmask;
}

static void slideshow_btn0_isr(){
    if(millis() - basic_io::btn0_last_pressed < 200) return;
    #if LOG == true
        call( "slideshow_btn0_isr");
    #endif
    basic_io::btn0_last_pressed = millis();
    btn_pressed |= basic_io::btn0_bmask;
}

void slideshow_menuconfig_mode(){
    /// [4] UP
    /// [3] DN
    /// [2] +
    /// [1] -
    /// [0] OK

    #if LOG == true
        call("slideshow_menuconfig_mode");
    #endif
    #if TFT_SCREEN == true
        btn_pressed = 0x0;

        while(basic_io::btn1_val() == false) basic_io::led0_blinky(5);

        const uint8_t   title0 = 2;
        const uint16_t  btn_pressedBox_W = 68,      btn_pressedBox_H = 31,      btn0 = 190,
                        row_distance = 23,          x_option = title0+65,       max_sel = 7;
        uint16_t        sel = 0,                    prev_sel = 2;
        uint32_t        last_t = 0;

        while (0x1){ /// menuconfig - main loop
            /// UP
            if( btn_pressed & basic_io::btn4_bmask ){
                btn_pressed &= basic_io::btn4_invbmask;
                sel = (sel == 0) ? (max_sel-1) : (sel-1);
            }
            /// DOWN
            if( btn_pressed & basic_io::btn3_bmask){ 
                btn_pressed &= basic_io::btn3_invbmask;
                sel = (sel == max_sel - 1) ? (0) : (sel + 1);
            }
            /// increase
            if( btn_pressed & basic_io::btn2_bmask ){
                btn_pressed &= basic_io::btn2_invbmask;
                switch (sel){
                case 0:
                    delay0.set_interval(num_increase(delay0.get_interval(), 5000, 0, 6000000));
                    delay(100);
                    break;
                case 1:
                    delay(300);
                    show_env_info = ! show_env_info;
                    break;
                case 2:
                    delay1.set_interval(num_increase(delay1.get_interval(), 5000, 0, 6000000));
                    delay(100);
                    break;
                case 3:
                    env_box_pos.X() = num_increase(env_box_pos.X(), 2, 2, 174);
                    delay(100);
                    break;
                case 4:
                    env_box_pos.Y() = num_increase(env_box_pos.Y(), 2, 2, 50);
                    delay(100);
                    break;
                case 5:
                    delay(300);
                    turn_on_bedroom_light = ! turn_on_bedroom_light;
                    break;
                case 6:
                    delay(100);
                    light_level = num_increase(light_level, 5, 0, 100);
                }
            }
            /// decrease
            if(btn_pressed & basic_io::btn1_bmask){
                btn_pressed &= basic_io::btn1_invbmask;
                switch (sel){
                case 0:
                    delay0.set_interval(num_decrease(delay0.get_interval(), 5000, 0, 6000000));
                    delay(100);
                    break;
                case 1:
                    delay(300);
                    show_env_info = ! show_env_info;
                    break;
                case 2:
                    delay1.set_interval(num_decrease(delay1.get_interval(), 5000, 0, 6000000));
                    delay(100);
                    break;
                case 3:
                    env_box_pos.X() = num_decrease(env_box_pos.X(), 2, 2, 174);
                    delay(100);
                    break;
                case 4:
                    env_box_pos.Y() = num_decrease(env_box_pos.Y(), 2, 2, 50);
                    delay(100);
                    break;
                case 5:
                    delay(300);
                    turn_on_bedroom_light = ! turn_on_bedroom_light;
                    break;
                case 6:
                    delay(100);
                    light_level = num_decrease(light_level, 5, 0, 100);
                    break;
                }
            }
            /// OKE
            if( btn_pressed & basic_io::btn0_bmask ){
                btn_pressed &= basic_io::btn0_invbmask;
                goto MENUCONFIG_SLIDESHOW_SAFE_EXIT;
            }

            DISPLAY_MENUCONFIG:
            /// clear canvas
            canvas.refill(0xFFFF);
            /// show title
            canvas.insert_rectangle(POINT<>(title0-1, 1), 170, 35, 0x18c3, true, sensors_color_1);
            canvas.insert_text(POINT<>(title0+22, 45), "menuconfig", sensors_color_2);
            /// show guide
            show_GUIDE("UP", "DN", "  +", "  -", "OK");
            /// show option - slideshow duration
            show_MENUCONFIG_LINE(x_option + uint16_t(0)*row_distance, "Img/Interval:", delay0.get_interval()/1000);
            /// show option - show temp,humid
            show_MENUCONFIG_LINE(x_option + uint16_t(1)*row_distance, "Env/Enable:", show_env_info?"Y":"N");
            show_MENUCONFIG_LINE(x_option + uint16_t(2)*row_distance, "Env/Interval:", delay1.get_interval()/1000);
            show_MENUCONFIG_LINE(x_option + uint16_t(3)*row_distance, "Env/Pos/Row:", env_box_pos.X());
            show_MENUCONFIG_LINE(x_option + uint16_t(4)*row_distance, "Env/Pos/Col:",  env_box_pos.Y());

            /// show option - show bedroom led light
            show_MENUCONFIG_LINE(x_option + uint16_t(5)*row_distance, "Light/Enable:", turn_on_bedroom_light?"Y":"N");
            show_MENUCONFIG_LINE(x_option + uint16_t(6)*row_distance, "Light/Level:", light_level);

            /// show current selection
            canvas.insert_rectangle(POINT<>(x_option-17 + sel * row_distance, 2), 141, 25, SELECTED_BOX, false, 0x0, 0x8);

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
    /// [3] : prev image
    /// [2] : light control
    /// [1] : env info control
    /// [0] : enter menu mode / oke 

    #if LOG == true
        call( "slideshow_mode");
    #endif

    single_screen_color_and_text_line(1, "[slideshow]", 0xFFFF, 0x0, false);
    single_screen_color_and_text_line(2, "loading...", 0xFFFF, 0x0, true, false);

    basic_io::btn4_attach_interrupt(slideshow_btn4_isr);
    basic_io::btn3_attach_interrupt(slideshow_btn3_isr);
    basic_io::btn2_attach_interrupt(slideshow_btn2_isr);
    basic_io::btn1_attach_interrupt(slideshow_btn1_isr);
    basic_io::btn0_attach_interrupt(slideshow_btn0_isr);

    #if TFT_SCREEN == true

        if( sdcard_imgs::list() == 0 )
            sdcard_imgs::is_available = false;

        uint8_t     title0 = 2;
        uint16_t    btn_pressedBox_W = 68, btn_pressedBox_H = 31, btn0 = 190,
                    img_pos = 0, sel = 0, prev_sel = 2;

        get_and_show_image(img_pos, false);

        while(0x1){ /// slideshow - main loop
            SLIDESHOW:
            
            /// set bedroom led light
            #if BASIC_IO == true
                if(turn_on_bedroom_light){
                    basic_io::led1_analog_ctl(light_level);
                }else{
                    basic_io::led1_analog_ctl(0);
                }
            #endif

            /// @brief for slideshow image control :v
            /// @brief -> next
            if( btn_pressed & basic_io::btn4_bmask){
                btn_pressed &= basic_io::btn4_invbmask;
                if(sel == 0){
                    get_and_show_image(img_pos, true, true, false);
                    goto SHOW_CHANGED;
                }
            }

            /// @brief -> previous
            if( btn_pressed & basic_io::btn3_bmask ){ 
                btn_pressed &= basic_io::btn3_invbmask;
                if(sel == 0){
                    get_and_show_image(img_pos, true, false, false);
                    goto SHOW_CHANGED;
                }
            }

            /// left / DN-mode
            if( btn_pressed & basic_io::btn2_bmask ){
                btn_pressed &= basic_io::btn2_invbmask;
                switch (sel){
                /// increase light level
                case 0: 
                    light_level = (light_level > 100) ? 0 :
                                  (light_level == 0 ) ? 100 : (light_level-10);
                    break;
                /// control selected box 
                case 1:
                    sel = 0; prev_sel = 1;
                    goto RE_DRAW;
                case 2:
                    sel = 1; prev_sel = 2;
                    break;
                }
            }

            /// right / UP-mode
            if( btn_pressed & basic_io::btn1_bmask ){
                btn_pressed &= basic_io::btn1_invbmask;
                switch (sel){
                /// toggle inv box show state
                case 0: 
                    if(show_env_info = !show_env_info); goto RE_DRAW;
                /// control selected box 
                case 1:
                    sel = 2; prev_sel = sel-1;
                    break;
                case 2:
                    sel = 0; prev_sel = 0;
                    goto RE_DRAW;
                    break;
                }
            }

            if( btn_pressed & basic_io::btn0_bmask ){
                btn_pressed &= basic_io::btn0_invbmask;
                switch (sel){
                /// toggle guide mode
                case 0: 
                    sel = (sel+1) % 3;
                    break;
                /// enter selection
                case 1:
                    /// goto menuconfig mode
                    slideshow_menuconfig_mode();
                    RE_DRAW:
                    /// re-draw img
                    sel = 0; prev_sel = sel-1;
                    get_and_show_image(img_pos, false); 
                    goto SHOW_CHANGED;
                case 2:
                    screen_mode = enum_SCREEN_MODE::NORMAL_MODE;
                    goto SLIDESHOW_SAFE_EXIT;
                }
            }


            /// periody update env info
            if( sel == 0 && delay1.time_to_run() ){
                humid = sensors::read_humid(0);
                temp = sensors::read_temp(0);
            }

            /// periody update image
            if( sel == 0 &&  delay0.time_to_run(true) ){
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
                show_GUIDE("NEXT", "PREV", "ENV", "LIGHT", "MENU", 65, 100);
                show_GUIDE("/", "/", "/", "/", "/", 65, 125);
                show_GUIDE("--", "--", "<-", "->", "OK");
                /// show next/mode button
                show_2button_on_1line( btn0, "Setup", 25, "Exit", 110, sensors_color_4, sensors_color_6, sensors_color_5, sensors_color_7 );
                /// show btn_pressed box based on ```sel```
                switch (sel){
                    case 1:
                        canvas.insert_rectangle(POINT<>(btn0-2, 13), btn_pressedBox_W, btn_pressedBox_H, sensors_color_21, false, 0x0, 0xF);
                        break;
                    case 2:
                        canvas.insert_rectangle(POINT<>(btn0-2, 91), btn_pressedBox_W, btn_pressedBox_H, sensors_color_21, false, 0x0, 0xF);
                        break;
                }
                prev_sel = sel;                                                       
            }
            SHOW_CHANGED:
            canvas.show();
        }
    #endif
    SLIDESHOW_SAFE_EXIT:
    log2ser("goto SLIDESHOW_SAFE_EXIT");
    btn_pressed = 0x0;
    screen_mode = enum_SCREEN_MODE::NORMAL_MODE;
    basic_io::btn4_detach_interrupt();
    basic_io::btn3_detach_interrupt();
    basic_io::btn2_detach_interrupt();
    basic_io::btn1_detach_interrupt();
    basic_io::btn0_detach_interrupt();
    return;
}



/// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
/// >>>>>                                                      >>>>>
/// >>>>>                                                      >>>>>
/// >>>>>                                                      >>>>>
/// >>>>>                                                      >>>>>
/// >>>>>>>>>>>>>>>>>>>>> NORMAL MODE >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
void normal_mode(){
    call("normal_mode");
    while(0x1) reserved_feature_mode();
}



/// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
/// >>>>>                                                      >>>>>
/// >>>>>                                                      >>>>>
/// >>>>>                                                      >>>>>
/// >>>>>                                                      >>>>>
/// >>>>>>>>>>>>>>>>>>>>> TEST MODE >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
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