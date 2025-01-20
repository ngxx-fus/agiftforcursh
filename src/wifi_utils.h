#ifndef WIFI_UTILS_H
#define WIFI_UTILS_H

#define elif else if

#include "WiFi.h"
#if FIREBASE_RTDB == true
    #include "FirebaseESP32.h"
#endif

#include "basic_io_utils.h"
#include "serial_utils.h"
#include "tft_utils.h"
#include "images.h"

#if FIREBASE_RTDB == true
    const char PROGMEM  DATABASE_URL[] =    "https://agiftforcrush-default-rtdb.firebaseio.com/";
    const char PROGMEM  API_KEY[]      =    "AIzaSyD2KF4BJw4Ye3aDAzs8P4K9Bj7D_dOevWM";
    const char PROGMEM EMAIL[]         =    "prototype-account@agiftforcrush.iam.gserviceaccount.com";
    const char PROGMEM PASSWORD[]      =    "prototype@pw3204";

    FirebaseData firebaseData;
    FirebaseAuth auth;
    FirebaseConfig config;
#endif

    /// try to connect manufacture wifi
static const char PROGMEM   mSSID[] = "hg.xnb",
                            mPW[]   = "nGXXFUS@3204";

#define SHOW_GUIDE_COUNTS_MAX 100
#ifndef BTN_PRESSED
    #define BTN_PRESSED
    static uint8_t btn_pressed = 0x0;
#endif

static void wifi_setup_btn3_isr(){
    #if LOG == true
        call( "wifi_setup_btn3_isr");
    #endif
    btn_pressed |= 0x8;
}

static void wifi_setup_btn2_isr(){
    #if LOG == true
        call( "wifi_setup_btn2_isr");
    #endif
    btn_pressed |= 0x4;
}

static void wifi_setup_btn1_isr(){
    #if LOG == true
        call( "wifi_setup_btn1_isr");
    #endif
    btn_pressed |= 0x2;
}

static void wifi_setup_btn0_isr(){
    #if LOG == true
        call( "wifi_setup_btn0_isr");
    #endif
    btn_pressed |= 0x1;
}

static void show_GUIDE(String g3 = "UP", String g2 = "DN", String g1 = "CH", String g0 = "OK"){
    /// show guide
    canvas.insert_text(POINT<>(85, 145),    g3, GUIDE_COLOR);
    canvas.insert_text(POINT<>(85+30, 145), g2, GUIDE_COLOR);
    canvas.insert_text(POINT<>(85+60, 145), g1, GUIDE_COLOR);
    canvas.insert_text(POINT<>(85+90, 145), g0, GUIDE_COLOR);
}

#if FIREBASE_RTDB == true
    bool connect_to_rtdb_firebase(){
        #if LOG == true
            call("connect_to_rtdb_firebase");
            log2ser( "Connecting to Firebase...");
        #endif

        if(WiFi.status() != WL_CONNECTED){
            #if LOG == true
                log2ser( "Not Internet Connection!");
            #endif
            goto CONNECT_TO_RTDB_FIREBASE_SAFE_EXIT;
        }

        config.api_key = (String) API_KEY;
        config.database_url = (String) DATABASE_URL;
        auth.user.email = String(EMAIL);
        auth.user.password = (String) PASSWORD;
        firebaseData.setBSSLBufferSize(512, 512);
        firebaseData.setResponseSize(512);
        firebaseData.setCert(NULL);
        // firebaseData.setReadTimeout(firebaseData, 5000);
        for(uint8_t i = 0; i < 3; ++i) {
            #if BASIC_IO == true
                basic_io::led0_blinky(2);
            #endif
            #if LOG == true
                log2ser("ESP.getFreeHeap: ", ESP.getFreeHeap());
            #endif
            Firebase.begin(&config, &auth);
            #if LOG == true
                log2ser("ESP.getFreeHeap: ", ESP.getFreeHeap());
            #endif
            if(Firebase.ready()) {
                Firebase.reconnectWiFi(true);
                #if BASIC_IO == true
                    basic_io::led1_blinky(2);
                #endif
                #if LOG == true
                    msg2ser(  "Connected to Firebase!");
                #endif
                return true;
            }
        }
        CONNECT_TO_RTDB_FIREBASE_SAFE_EXIT:
        #if LOG == true
            msg2ser(  "Can't connect to Firebase!");
        #endif
        return false;
    }
#endif

uint8_t wifi_connect(uint8_t wifi_i = 0){

    // call("wifi_connect");
    // log2ser( "SSID: ", WiFi.SSID(wifi_i));
    // log2ser( "RSSI: ", WiFi.RSSI(wifi_i));
    // log2ser( "ENCRYPT: ", WiFi.encryptionType(wifi_i));

    // /// Position
    // uint8_t wifi_icon = 50,     net0 = 2, 
    //         box_row, box_col,   box_w, box_h;
    // uint8_t sel = 0,            sel_c = 0,
    //         wf_ss_level = 0;
    // bool    caps_lock = false;
    // String  PASSWORD = "password";
    
    // /// Repeat until have selected from user
    // while(0x1){
    //     /// Clear canvas
    //     canvas.clear();
    //     /// Show WiFi SSID
    //     /// Decide WiFi icon based on RSSI
    //     if      (WiFi.RSSI(wifi_i) >= -30)  wf_ss_level = 0;
    //     else if (WiFi.RSSI(wifi_i) >= -67)  wf_ss_level = 1;
    //     else if (WiFi.RSSI(wifi_i) >= -70)  wf_ss_level = 2;
    //     else                                wf_ss_level = 3;
    //     /// Draw Wi-Fi Icon + SSID + RSSI
    //     canvas.insert_rectangle(POINT<>(net0-1, 1), 170, 27, 0x0, true, 0x08ab);
    //     canvas.insert_bitmap_image(POINT<>(net0+2, 3), sign_wifi_icon[wf_ss_level], 23, 17, 0xfecd);
    //     canvas.insert_text(POINT<>(net0+18, 28), WiFi.SSID(wifi_i), 0xfecd);
    //     /// Re-scan & Skip button
    //     show_2button_on_1line(190, "Exit", 29, "Next", 108);
    //     /// Show password box
    //     show_input_box(40, "password", PASSWORD, 33840U, 23275U, 23275U, (caps_lock)?0xfc07:0x0088);
    //     /// Show keyboard
    //     show_keyboard(90);
    //     /// Computing box_row based on ```sel```
    //     switch (sel){
    //         case 0: box_row = 40+7  ; break;
    //         case 1: box_row = 90-14 ; break;
    //         case 2: box_row = 108-14; break;
    //         case 3: box_row = 126-14; break;
    //         case 4: box_row = 144-14; break;
    //         case 5: box_row = 162-14; break;
    //         case 6: box_row = 180-14; break;
    //         case 7: box_row = 190-1 ; break;
    //     }
    //     /// Computing box_col, box_w, box_h based on ```sel```
    //     /// and ```sel_c```
    //     if(sel == 0) {box_col = 137, box_w = 28, box_h = 20;}
    //     if(sel >= 1 && sel <= 5){
    //         box_col = 5 + sel_c * 14, box_w = 10, box_h = 20;
    //     }
    //     if(sel == 6){
    //         if(sel_c <= 8){
    //             box_col = 5 + sel_c * 14, box_w = 10, box_h = 20;
    //         }else{
    //             box_row += 2;
    //             box_col = 148, box_w = 14, box_h = 17;
    //         }
    //     }
    //     if(sel == 7){
    //         if(sel_c == 0){
    //             box_col = 14, box_w = 66, box_h = 29;
    //         }elif(sel_c == 1){
    //             box_col = 92, box_w = 66, box_h = 29;
    //         }else{
    //             sel_c == 0, box_col = 14, box_w = 66, box_h = 29;
    //         }
    //     }
    //     /// Insert selected box
    //     canvas.insert_rectangle(POINT<>(box_row, box_col), box_w, box_h, 0xe248);
    //     canvas.show();
    //     /// control
    //     if(in_range(x_adc_value(), 0, 30)){
    //         sel = (sel+7)%8;
    //         while(in_range(x_adc_value(), 0, 30));
    //     }
    //     if(in_range(x_adc_value(), 200, 255)){
    //         sel = (sel+1)%8;
    //         if(sel == 7) sel_c = 0;
    //         while(in_range(x_adc_value(), 200, 255));
    //     }
    //     if(sel > 0 && in_range(y_adc_value(), 0, 30)){
    //         if(sel == 7)
    //             sel_c = (sel_c+1)%2;
    //         else if(sel == 6)
    //             sel_c = (sel_c+9)%10;
    //         else
    //             sel_c = (sel_c+11)%12;
    //         while(in_range(y_adc_value(), 0, 30));
    //     }
    //     if(sel > 0 && in_range(y_adc_value(), 200, 255)){
    //         if(sel == 7)
    //             sel_c = (sel_c+1)%2;
    //         else if(sel == 6)
    //             sel_c = (sel_c+1)%10;
    //         else
    //             sel_c = (sel_c+1)%12;
    //         while(in_range(y_adc_value(), 200, 255));
    //     }
    //     /// process when sw is pressed
    //     if(sw_value() == LOW){
    //         /// delete button
    //         if( sel == 0 && !PASSWORD.isEmpty()){
    //             PASSWORD.remove(PASSWORD.length()-1);
    //         }
    //         /// exit button
    //         if( sel == 7 && sel_c == 0) {
    //             log2ser( "return: 0x1");
    //             return 0x1;
    //         }
    //         /// next connect) button
    //         if( sel == 7 && sel_c == 1){
    //             /// Clear sreen for show connection status
    //             canvas.clear();
    //             /// Decide WiFi icon based on RSSI
    //             if      (WiFi.RSSI(wifi_i) >= -30)  wf_ss_level = 0;
    //             else if (WiFi.RSSI(wifi_i) >= -67)  wf_ss_level = 1;
    //             else if (WiFi.RSSI(wifi_i) >= -70)  wf_ss_level = 2;
    //             else                                wf_ss_level = 3;
    //             /// Show WiFi SSID 
    //             canvas.insert_rectangle(POINT<>(net0-1, 1), 170, 27, 0x0, true, 0x08ab);
    //             canvas.insert_bitmap_image(POINT<>(net0+2, 3), sign_wifi_icon[wf_ss_level], 23, 17, 0xfecd);
    //             canvas.insert_text(POINT<>(net0+18, 28), WiFi.SSID(wifi_i), 0xfecd);
    //             canvas.show();
    //             /// Connect to Wi-Fi
    //             WiFi.begin(WiFi.SSID(wifi_i), PASSWORD);
    //             /// Wait for Wi-Fi util connect
    //             uint8_t connecting_row = net0+40;
    //             do{
    //                 canvas.insert_text(POINT<>(connecting_row, 5), "Connecting...", 0x90dc);
    //                 connecting_row += 18;
    //                 canvas.show();
    //                 delay(2000);
    //             }while(connecting_row < 182 && WiFi.status() != WL_CONNECTED);
    //             /// Return 
    //             if(WiFi.status() == WL_CONNECTED){ 
    //                 canvas.insert_text(POINT<>(connecting_row, 5), "Connected!", 0x0407);
    //                 canvas.show();
    //                 delay(1000);

    //                 screen_mode = enum_SCREEN_MODE::NORMAL_MODE;
    //                 log2ser( "WiFi: connected!");
    //                 log2ser( "return: 0x0");

    //                 return 0x0; // connect successfull
    //             }
    //             canvas.insert_text(POINT<>(connecting_row, 5), "Failed to connect!", 0xe843);
    //                 connecting_row += 18;
    //             canvas.insert_text(POINT<>(connecting_row, 5), "Return to Wi-Fi scan...", 0xe843);
    //             canvas.show();
    //             delay(1000);
                
    //             log2ser( "WiFi: can't connect!");
    //             log2ser( "return: 0x2");

    //             return 0x2; // failed to connect wifi
    //         }
    //         /// insert a character into PASSWORD
    //         if( 1 <= sel && sel <=5 ){
    //             PASSWORD += String(char(
    //                 (caps_lock==true && 
    //                 'a' <= keyboard[sel-1][sel_c] && 
    //                 keyboard[sel-1][sel_c] <= 'z')
    //                 ?(keyboard[sel-1][sel_c] - ('z'-'Z'))
    //                 :keyboard[sel-1][sel_c]
    //             ));
    //         }
    //         /// insert a character into PASSWORD
    //         if( sel == 6 && sel_c < 9 ){
    //             PASSWORD += String(char(
    //                 (caps_lock==true && 
    //                 'a' <= keyboard[sel-1][sel_c] && 
    //                 keyboard[sel-1][sel_c] <= 'z')
    //                 ?(keyboard[sel-1][sel_c] - ('z'-'Z'))
    //                 :keyboard[sel-1][sel_c]
    //             ));
    //         }
    //         /// toggle caps_lock state
    //         if( sel == 6 && sel_c == 9 ){
    //             caps_lock = !caps_lock;
    //         }
    //         while(sw_value() == LOW);
    //     }

    // } 
    
    // /// By the normal, this function can't reach here :v
    // screen_mode = enum_SCREEN_MODE::ERROR_MODE;
    log2ser( "return: 0xff");
    return 0xff;
}

void wifi_setup(){
    call("wifi_setup: ");

    WiFi.mode(WIFI_STA);

    #if LOG == true
        log2ser("Try connect to Manufacturer's Wi-Fi...");
    #endif
    
    for(uint8_t i = 0; i <= 2; ++i){
        #if BASIC_IO == true
            basic_io::led0_blinky(2);
        #endif
        WiFi.begin(mSSID, mPW);
        delay(1000);
        #if FIREBASE_RTDB == true
            if(connect_to_rtdb_firebase()) return;
        #endif
    }

    #if LOG == true
        log2ser("Failed!");
        log2ser("Scanning other Wi-Fis...");
    #endif

    if(WiFi.status() == WL_CONNECTED) return;

    /// scan and connect to another wifi
    basic_io::led0_blinky(5, 5, 15);
    basic_io::btn3_attach_interrupt(wifi_setup_btn3_isr);
    basic_io::btn2_attach_interrupt(wifi_setup_btn2_isr);
    basic_io::btn1_attach_interrupt(wifi_setup_btn1_isr);
    basic_io::btn0_attach_interrupt(wifi_setup_btn0_isr);

    screen_mode = enum_SCREEN_MODE::SETUP_WIFI_MODE;

    uint8_t text0 = 1, wifi_icon = 50, btn0 = 130, btn1 = btn0 + 30,
            net0 = 0, net1 = 0, net2 = 0, net3 = 0, net4 = 0;
    uint8_t sel = false, prev_sel = true;
    uint8_t wf_ss_level = 0;
    uint8_t show_guide_count = 0;

    /// choose START_SCAN or SKIPP
    while(0x1){
        if(sel != prev_sel){
            prev_sel = sel;
            canvas.clear();
            /// Show skip or scan question?
            canvas.insert_rectangle(POINT<>(text0, 1), 170, 27, 0x0, true, 0xf7be);
            canvas.insert_text(POINT<>(text0+18, 35), "Set-up Wi-Fi?", 0x2945);
            /// WiFi Icon
            canvas.insert_bitmap_image(POINT<>(wifi_icon, 50), wifi_icon70x53, 70, 53, 0x0964);
            /// WiFi btn0
            canvas.insert_bitmap_image(POINT<>(btn0, 26), _119x27_rounded_rectangle, 119, 27, 0x3d3b);
            canvas.insert_text(POINT<>(btn0+18, 56), "Scan [>]", 0xFFFF);
            /// WiFi btn1
            canvas.insert_bitmap_image(POINT<>(btn1, 26), _119x27_rounded_rectangle, 119, 27, 0xf2b2);
            canvas.insert_text(POINT<>(btn1+18, 56), "Skip [>]", 0xFFFF);
            /// Selection box
            if(sel == 0)
                canvas.insert_rectangle(POINT<>(btn0, 26) - POINT<>(1,2), 121, 29, 0x630c);
            else
                canvas.insert_rectangle(POINT<>(btn1, 26) - POINT<>(1,2), 121, 29, 0x630c);

            canvas.show();
        }
        /// show guide
        if(show_guide_count < SHOW_GUIDE_COUNTS_MAX){
            /// log2ser("show guide (",show_guide_count,")");
            ++show_guide_count;
            show_GUIDE("UP", "DN", "OK", "?");
            canvas.show();
        }
        /// move up selection
        if(btn_pressed & 0x8){
            btn_pressed = btn_pressed & 0xF7;
            sel = (sel)?0:1;
            delay(200);
        }

        /// move down selection
        if( btn_pressed & 0x4 ){ 
            btn_pressed = btn_pressed & 0xFB;
            sel = (sel)?0:1;
            delay(200);
        }

        if( btn_pressed & 0x2 ){ 
            btn_pressed = btn_pressed & 0xFD;
            if(sel == 1){
                screen_mode = enum_SCREEN_MODE::NORMAL_MODE;
                return;
            }else{
                break;
            }
            delay(200);
        }
    }

    /// reset show guide count
    show_guide_count = 0;
    /// SCAN and CHOOSE Wi-Fi access-point
    prev_sel = ~sel;
    /// move btn0 and 1
    btn0 += 30; btn1 += 30;
    START_WIFI_SCANNING:
    while(0x1){
        /// SCANNING Wi-Fi
        canvas.clear();
        canvas.insert_rectangle(POINT<>(text0, 1), 170, 27, 0x0, true, 0xf7be);
        canvas.insert_text(POINT<>(text0+18, 21), "Scanning Wi-Fi...", 0x2945);
        canvas.show();

        uint16_t t0 = millis();
        log2ser( "Wi-Fi: Scanning AP...");
        WiFi.disconnect(false, true);
        WiFi.mode(WIFI_STA);
        WiFi.scanDelete();
        int16_t nets =  WiFi.scanNetworks();
        if(t_since(t0) <= 1000){
            log2ser( "Wi-Fi scanning is too fast!");
            screen_mode = enum_SCREEN_MODE::ERROR_MODE;
            goto WIFI_SETUP_SAFE_EXIT;
        }
        
        /// SELECT Wi-Fi to connect
        
        /// By default: net0 is selected
        sel = 0;
        /// move net[0->4] (in other words, set position for 5 Wi-Fi access points)
        net0 = 32, net1 = net0 + 26, net2 = net1 + 26, net3 = net2 + 26, net4 = net3 + 26;
        /// By default, the first WiFi access point is selected
        uint8_t sel_row = 0; 
        /// Reapeat until has any selection (by press SW)
        while(0x1){
            /// Show all available Wi-Fi(s)
            canvas.clear();
            /// show guide
            if(show_guide_count <= SHOW_GUIDE_COUNTS_MAX){
                /// log2ser("show guide (",show_guide_count,")");
                ++show_guide_count;
                show_GUIDE("UP", "DN", "OK", "?");
                // canvas.show();
            }
            canvas.insert_rectangle(POINT<>(text0, 1), 170, 27, 0x08ab, true, 0x08ab);
            canvas.insert_text(POINT<>(text0+18, 17), "Available Wi-Fi(s)", 0xFFFF);
            /// net 0
            if(nets > 0){
                if(WiFi.RSSI(0) >= -30) wf_ss_level = 0;
                else if(WiFi.RSSI(0) >= -67) wf_ss_level = 1;
                else if(WiFi.RSSI(0) >= -70) wf_ss_level = 2;
                else wf_ss_level = 3;
                canvas.insert_bitmap_image(POINT<>(net0, 3), sign_wifi_icon[wf_ss_level], 23, 17, 0x1087);
                canvas.insert_text(POINT<>(net0+15, 28), WiFi.SSID(0), 0x1087);
            }
            /// net 1
            if(nets > 1){
                if(WiFi.RSSI(1) >= -30) wf_ss_level = 0;
                else if(WiFi.RSSI(1) >= -67) wf_ss_level = 1;
                else if(WiFi.RSSI(1) >= -70) wf_ss_level = 2;
                else wf_ss_level = 3;
                canvas.insert_bitmap_image(POINT<>(net1, 3), sign_wifi_icon[wf_ss_level], 23, 17, 0x1087);
                canvas.insert_text(POINT<>(net1+15, 28), WiFi.SSID(1), 0x1087);
            }
            /// net 2
            if(nets > 2){
                if(WiFi.RSSI(2) >= -30) wf_ss_level = 0;
                else if(WiFi.RSSI(2) >= -67) wf_ss_level = 1;
                else if(WiFi.RSSI(2) >= -70) wf_ss_level = 2;
                else wf_ss_level = 3;
                canvas.insert_bitmap_image(POINT<>(net2, 3), sign_wifi_icon[wf_ss_level], 23, 17, 0x1087);
                canvas.insert_text(POINT<>(net2+15, 28), WiFi.SSID(2), 0x1087);
            }
            /// net 3
            if(nets > 3){
                if(WiFi.RSSI(3) >= -30) wf_ss_level = 0;
                else if(WiFi.RSSI(3) >= -67) wf_ss_level = 1;
                else if(WiFi.RSSI(3) >= -70) wf_ss_level = 2;
                else wf_ss_level = 3;
                canvas.insert_bitmap_image(POINT<>(net3, 3), sign_wifi_icon[wf_ss_level], 23, 17, 0x1087);
                canvas.insert_text(POINT<>(net3+15, 28), WiFi.SSID(3), 0x1087);
            }
            /// net 4
            if(nets > 4 ){
                if(WiFi.RSSI(4) >= -30) wf_ss_level = 0;
                else if(WiFi.RSSI(4) >= -67) wf_ss_level = 1;
                else if(WiFi.RSSI(4) >= -70) wf_ss_level = 2;
                else wf_ss_level = 3;
                canvas.insert_bitmap_image(POINT<>(net4, 3), sign_wifi_icon[wf_ss_level], 23, 17, 0x1087);
                canvas.insert_text(POINT<>(net4+15, 28), WiFi.SSID(4), 0x1087);
            }
            /// rescan button
            canvas.insert_bitmap_image(POINT<>(btn0, 26), _119x27_rounded_rectangle, 119, 27, 0x3d3b);
            canvas.insert_text(POINT<>(btn0+18, 41), "Re-scan [>]", 0xFFFF);
            /// skip button
            canvas.insert_bitmap_image(POINT<>(btn1, 26), _119x27_rounded_rectangle, 119, 27, 0xf2b2);
            canvas.insert_text(POINT<>(btn1+18, 41), "Skip      [>]", 0xFFFF);
            /// calc selected row
            switch (sel){
                case 0: sel_row = net0; break;
                case 1: sel_row = net1; break;
                case 2: sel_row = net2; break;
                case 3: sel_row = net3; break;
                case 4: sel_row = net4; break;
                
                case 5: sel_row = btn0; break;
                case 6: sel_row = btn1; break;
            }
            /// show selected box
            if(sel < 5)
                canvas.insert_rectangle(POINT<>(sel_row-2, 1), 170, 25, 0xfaeb);
            else
                canvas.insert_rectangle(POINT<>(sel_row-1, 24), 121, 29, 0x12ca);
            /// show to screen
            canvas.show();
            /// change selected row (up)
            if(btn_pressed & 0x8){
                btn_pressed = btn_pressed & 0xF7;
                sel = (sel+6)%7;
                delay(200);
            }
            /// change selected row (down)
            if( btn_pressed & 0x4 ){ 
                btn_pressed = btn_pressed & 0xFB;
                sel = (sel+1)%7;
                delay(200);
            }
            /// process selected mode
            if( btn_pressed & 0x2 ){ 
                btn_pressed = btn_pressed & 0xFD;
                delay(200);
                /// sel: 0..4
                if( nets > 0 && sel < 5){
                    uint8_t return_code =  wifi_connect(sel);
                    switch (return_code){
                        /// Connected to Wi-Fi 
                        case 0: return;
                        /// Exit (without connect wifi) code
                        case 1: goto START_WIFI_SCANNING;
                        /// Failed to connect to Wi-Fi
                        case 2: 
                            log2ser("goto ```START_WIFI_SCANNING```");
                            goto START_WIFI_SCANNING; 
                    };
                }
                /// sel: 5
                if(sel == 5) {
                    goto START_WIFI_SCANNING;
                }
                /// sel: 6
                if(sel == 6){
                    screen_mode = enum_SCREEN_MODE::NORMAL_MODE ; 
                    goto WIFI_SETUP_SAFE_EXIT;
                }
            }
        }
    }

    WIFI_SETUP_SAFE_EXIT:
    btn_pressed = 0x0;
    basic_io::btn3_detach_interrupt();
    basic_io::btn2_detach_interrupt();
    basic_io::btn1_detach_interrupt();
    basic_io::btn0_detach_interrupt();
    return;
}


#endif