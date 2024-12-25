#ifndef WIFI_UTILS_H
#define WIFI_UTILS_H
#include "controller_utils.h"
#include "serial_utils.h"
#include "tft_utils.h"
#include "images.h"
#include "WiFi.h"

const char keyboard[6][12] PROGMEM = {
    {'1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '+'},
    {'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']'},
    {'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\"','\''},
    {'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', ';', '/','\\'},
    {'`', '~', '!', '@', '#', '$', '%', '^', '&', '*', '(',')'},
    {'{', '}', '|', '?', '<', '>', '_', '=', ' ', ' ', ' ', ' '}
};

uint8_t wifi_connect(uint8_t wifi_i = 0){
    /// Position
    uint8_t pw_box0 = 40, wifi_icon = 50, btn0 = 190, 
            btn1 = btn0 + 30, net0 = 2, box_row, box_col, box_w, box_h,
            keyboard_row0 = 90, keyboard_row1 = 108, keyboard_row2 = 126,
            keyboard_row3 = 144, keyboard_row4 = 162, keyboard_row5 = 180;
    uint8_t wf_ss_level = 0;
    uint8_t sel = 0, prev_sel = 0xFF;
    uint8_t sel_c = 0, prev_sel_c = 0xFF;
    uint caps_lock = false;
    String PASSWORD = "password";
    start_draw_wifi_connect:
    /// Clear canvas
    canvas.clear();
    /// Show WiFi SSID
    if(WiFi.RSSI(wifi_i) >= -30) wf_ss_level = 0;
    else if(WiFi.RSSI(wifi_i) >= -67) wf_ss_level = 1;
    else if(WiFi.RSSI(wifi_i) >= -70) wf_ss_level = 2;
    else wf_ss_level = 3;
    canvas.insert_rectangle(POINT<>(net0-1, 1), 170, 27, 0x0, true, 0x08ab);
    canvas.insert_bitmap_image(POINT<>(net0+2, 3), sign_wifi_icon[wf_ss_level], 23, 17, 0xfecd);
    canvas.insert_text(POINT<>(net0+18, 28), WiFi.SSID(wifi_i), 0xfecd);
    /// Re-scan & Skip button
    canvas.insert_bitmap_image(POINT<>(btn0, 15), _64x27_rounded_rectangle, 64, 27, 0xf2b2);
    canvas.insert_text(POINT<>(btn0+18, 29), "Exit", 0xFFFF);
    canvas.insert_bitmap_image(POINT<>(btn0, 93), _64x27_rounded_rectangle, 64, 27, 0x3d3b);
    canvas.insert_text(POINT<>(btn0+18, 108), "Next", 0xFFFF);
    /// Show password box 
    canvas.insert_rectangle(POINT<>(pw_box0, 1), 170, 32, 0x8430);
    canvas.insert_line(POINT<>(pw_box0, 10), POINT<>(pw_box0, 10+80), 0xFFFF);
    canvas.insert_bitmap_image(POINT<>(pw_box0+9, 140), _22x15_delete_icon, 22, 15, 0x5aeb);
    canvas.insert_text(POINT<>(pw_box0+3, 15), "password", 0x4a49);
    /// show keyboard
    rept(uint16_t, c, 0, 11)
        canvas.insert_text(POINT<>(keyboard_row0, 5 + c * 14), String(keyboard[0][c]), 0x5aeb);
    rept(uint16_t, c, 0, 11)
        canvas.insert_text(POINT<>(keyboard_row1, 5 + c * 14), String(keyboard[1][c]), 0x5aeb);
    rept(uint16_t, c, 0, 11)
        canvas.insert_text(POINT<>(keyboard_row2, 5 + c * 14), String(keyboard[2][c]), 0x5aeb);
    rept(uint16_t, c, 0, 11)
        canvas.insert_text(POINT<>(keyboard_row3, 5 + c * 14), String(keyboard[3][c]), 0x5aeb);
    rept(uint16_t, c, 0, 11)
        canvas.insert_text(POINT<>(keyboard_row4, 5 + c * 14), String(keyboard[4][c]), 0x5aeb);
    rept(uint16_t, c, 0, 11)
        canvas.insert_text(POINT<>(keyboard_row5, 5 + c * 14), String(keyboard[5][c]), 0x5aeb);
    canvas.insert_bitmap_image(POINT<>(keyboard_row5-10, 150), _10x13_capslock_icon, 10, 13, 0x5aeb);

    /// show all
    canvas.show(true, true);
    while(0x1){
        /// show select box and password
        if(sel != prev_sel || sel_c != prev_sel_c){
            canvas.restore();
            canvas.insert_text(POINT<>(pw_box0+18, 4), PASSWORD, (caps_lock)?0xfc07:0x0088);

            switch (sel){
                case 0: box_row = pw_box0+7; break;
                case 1: box_row = keyboard_row0-14; break;
                case 2: box_row = keyboard_row1-14; break;
                case 3: box_row = keyboard_row2-14; break;
                case 4: box_row = keyboard_row3-14; break;
                case 5: box_row = keyboard_row4-14; break;
                case 6: box_row = keyboard_row5-14; break;
                case 7: box_row = btn0 - 1; break;
            }
            if(sel == 0) {box_col = 137, box_w = 28, box_h = 20;}
            if(sel >= 1 && sel <= 5){
                box_col = 5 + sel_c * 14, box_w = 10, box_h = 20;
            }
            if(sel == 6){
                if(sel_c <= 8){
                    box_col = 5 + sel_c * 14, box_w = 10, box_h = 20;
                }else{
                    box_row += 2;
                    box_col = 148, box_w = 14, box_h = 17;
                }
            }
            if(sel == 7){
                if(sel_c == 0){
                    box_col = 14, box_w = 66, box_h = 29;
                }
                if(sel_c == 1){
                    box_col = 92, box_w = 66, box_h = 29;
                }
            }
            canvas.insert_rectangle(POINT<>(box_row, box_col), box_w, box_h, 0xe248);
            canvas.show();
            prev_sel = sel, prev_sel_c = sel_c;
        }
        /// control
        if(in_range(x_adc_value(), 0, 30)){
            sel = (sel+7)%8;
            while(in_range(x_adc_value(), 0, 30));
        }
        if(in_range(x_adc_value(), 200, 255)){
            sel = (sel+1)%8;
            if(sel == 7) sel_c = 0;
            while(in_range(x_adc_value(), 200, 255));
        }
        if(sel > 0 && in_range(y_adc_value(), 0, 30)){
            if(sel == 7)
                sel_c = (sel_c+1)%2;
            else if(sel == 6)
                sel_c = (sel_c+9)%10;
            else
                sel_c = (sel_c+11)%12;
            while(in_range(y_adc_value(), 0, 30));
        }
        if(sel > 0 && in_range(y_adc_value(), 200, 255)){
            if(sel == 7)
                sel_c = (sel_c+1)%2;
            else if(sel == 6)
                sel_c = (sel_c+1)%10;
            else
                sel_c = (sel_c+1)%12;
            while(in_range(y_adc_value(), 200, 255));
        }
        /// process when sw is pressed
        while(sw_value() == LOW){
            if( sel == 0 && !PASSWORD.isEmpty()){
                PASSWORD.remove(PASSWORD.length()-1);
                prev_sel = ~sel; /// for reload screen :v
            }
            if( sel == 7 && sel_c == 0) return 0x1; 
            if( sel == 7 && sel_c == 1){
                canvas.clear(true);
                /// Show WiFi SSID
                if(WiFi.RSSI(wifi_i) >= -30) wf_ss_level = 0;
                else if(WiFi.RSSI(wifi_i) >= -67) wf_ss_level = 1;
                else if(WiFi.RSSI(wifi_i) >= -70) wf_ss_level = 2;
                else wf_ss_level = 3;
                canvas.insert_rectangle(POINT<>(net0-1, 1), 170, 27, 0x0, true, 0x08ab);
                canvas.insert_bitmap_image(POINT<>(net0+2, 3), sign_wifi_icon[wf_ss_level], 23, 17, 0xfecd);
                canvas.insert_text(POINT<>(net0+18, 28), WiFi.SSID(wifi_i), 0xfecd);
                canvas.show(true, true);
                /// Connect to Wi-Fi
                WiFi.begin(WiFi.SSID(wifi_i), PASSWORD);
                /// Wait for Wi-Fi util connect
                uint8_t connecting_row = net0+40;
                do{
                    canvas.insert_text(POINT<>(connecting_row, 5), "Connecting...", 0x90dc);
                    connecting_row += 18;
                    canvas.show();
                    delay(1000);
                }while(connecting_row < 200 && WiFi.status() != WL_CONNECTED);
                /// Return 
                if(WiFi.status() == WL_CONNECTED){ 
                    canvas.restore();
                    canvas.insert_text(POINT<>(connecting_row, 5), "Connected!", 0x0407);
                    canvas.show();
                    delay(1000);
                    return 0x0; // connect successfull
                }
                canvas.restore();
                canvas.insert_text(POINT<>(connecting_row, 5), "Return to Wi-Fi scan...", 0xe843);
                canvas.show();
                delay(1000);
                return 0x2; // failed to connect wifi
            }
            if( 1 <= sel && sel <=5 ){
                PASSWORD += String(char(
                    (caps_lock==true && 
                    'a' <= keyboard[sel-1][sel_c] && 
                    keyboard[sel-1][sel_c] <= 'z')
                    ?(keyboard[sel-1][sel_c] - ('z'-'Z'))
                    :keyboard[sel-1][sel_c]
                ));
                prev_sel = ~sel; /// for refresh screen :v
            }
            if( sel == 6 && sel_c < 9 ){
                PASSWORD += String(char(
                    (caps_lock==true && 
                    'a' <= keyboard[sel-1][sel_c] && 
                    keyboard[sel-1][sel_c] <= 'z')
                    ?(keyboard[sel-1][sel_c] - ('z'-'Z'))
                    :keyboard[sel-1][sel_c]
                ));
            }
            if( sel == 6 && sel_c == 9 ){
                caps_lock = !caps_lock;
                prev_sel = sel; /// for reload screen :v
            }
            while(sw_value() == LOW);
        }

    } 
    
    
    /// pause for debug
    while(0x1);

    /// normal connected to network
    return 0x0;
}

void wifi_setup(){
    uint8_t text0 = 1, wifi_icon = 50, btn0 = 130, btn1 = btn0 + 30,
            net0 = 0, net1 = 0, net2 = 0, net3 = 0, net4 = 0;
    uint8_t sel = false, prev_sel = true;
    uint8_t wf_ss_level = 0;
    canvas.restore();
    canvas.clear(true);

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
    /// Show
    canvas.show(true);

    while(0x1){
        if(sel != prev_sel){
            canvas.restore();
            if(sel == 0)
                canvas.insert_rectangle(POINT<>(btn0, 26) - POINT<>(1,2), 121, 29, 0x630c);
            else
                canvas.insert_rectangle(POINT<>(btn1, 26) - POINT<>(1,2), 121, 29, 0x630c);
            canvas.show();
            prev_sel = sel;
        }
        if(in_range(x_adc_value(), 0, 30)){
            sel = (sel)?0:1;
            while(in_range(x_adc_value(), 0, 30));
        }

        if(in_range(x_adc_value(), 200, 255)){
            sel = (sel)?0:1;
            while(in_range(x_adc_value(), 200, 255));
        }

        if(sw_value() == LOW){
            if(sel == 1) 
                return;
            else
                break;
            while(sw_value() == LOW);
        }
    }
    prev_sel = ~sel;
    /// move btn0 and 1
    btn0 += 30; btn1 += 30;
    while(0x1){
        start_wifi_scanning:
        canvas.restore();
        canvas.clear();
        canvas.insert_rectangle(POINT<>(text0, 1), 170, 27, 0x0, true, 0xf7be);
        canvas.insert_text(POINT<>(text0+18, 21), "Scanning Wi-Fi...", 0x2945);
        canvas.show();

        uint8_t nets =  WiFi.scanNetworks();
    
        canvas.restore();
        canvas.clear();
        canvas.insert_rectangle(POINT<>(text0, 1), 170, 27, 0x08ab, true, 0x08ab);
        canvas.insert_text(POINT<>(text0+18, 17), "Available Wi-Fi(s)", 0xFFFF);
        /// select net0
        sel = 0;
        /// move net[0->4]
        net0 = 32, net1 = net0 + 26, net2 = net1 + 26, net3 = net2 + 26, net4 = net3 + 26;
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
        canvas.show(true);
        
        uint8_t sel_row = 0;
        while(0x1){
            switch (sel){
                case 0: sel_row = net0; break;
                case 1: sel_row = net1; break;
                case 2: sel_row = net2; break;
                case 3: sel_row = net3; break;
                case 4: sel_row = net4; break;
                
                case 5: sel_row = btn0; break;
                case 6: sel_row = btn1; break;
            }
            if(prev_sel != sel){
                canvas.restore();
                if(sel < 5)
                    canvas.insert_rectangle(POINT<>(sel_row-2, 1), 170, 25, 0xfaeb);
                else
                    canvas.insert_rectangle(POINT<>(sel_row-1, 24), 121, 29, 0x12ca);
                canvas.show();
                prev_sel = sel;
            }
            if(in_range(x_adc_value(), 0, 30)){
                sel = (sel+6)%7;
                while(in_range(x_adc_value(), 0, 30));
            }

            if(in_range(x_adc_value(), 200, 255)){
                sel = (sel+1)%7;
                while(in_range(x_adc_value(), 200, 255));
            }

            if(sw_value() == LOW){
                if(sel == 5) goto start_wifi_scanning;
                if(sel == 6) return;
                if(sel < 5){
                    uint8_t return_code =  wifi_connect(sel);
                    switch (return_code){
                        case 0: return;
                        case 1: goto start_wifi_scanning;
                        case 2: goto start_wifi_scanning; /// failed to connect to Wi-Fi
                    };
                    return;
                }
                while(sw_value() == LOW);
            }
        }
    }
}


#endif