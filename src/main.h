#include <Arduino.h>

#define  elif else if

#define LOG true
#define SHOW_AFTER_CHANGED false

#include "controller_utils.h"
#include "serial_utils.h"
#include "tft_utils.h"
#include "images.h"
#include "WiFi.h"

/// @brief The canvas object
CANVAS<uint16_t> canvas;

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

    canvas.insert_bitmap_image(POINT<>(wifi_icon, 50), wifi_icon70x53, 70, 53, 0x0964);

    canvas.insert_bitmap_image(POINT<>(btn0, 26), rounded_rectangle, 119, 27, 0x3d3b);
    canvas.insert_text(POINT<>(btn0+18, 56), "Scan [>]", 0xFFFF);

    canvas.insert_bitmap_image(POINT<>(btn1, 26), rounded_rectangle, 119, 27, 0xf2b2);
    canvas.insert_text(POINT<>(btn1+18, 56), "Skip [>]", 0xFFFF);

    canvas.show(true);

    while(0x1){
        canvas.restore();
        if(sel == 0)
            canvas.insert_rectangle(POINT<>(btn0, 26) - POINT<>(1,2), 121, 29, 0x630c);
        else
            canvas.insert_rectangle(POINT<>(btn1, 26) - POINT<>(1,2), 121, 29, 0x630c);
        canvas.show();

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
        }
    }

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
            canvas.insert_text(POINT<>(net0+13, 28), WiFi.SSID(0), 0x1087);
        }
        /// net 1
        if(nets > 1){
            if(WiFi.RSSI(1) >= -30) wf_ss_level = 0;
            else if(WiFi.RSSI(1) >= -67) wf_ss_level = 1;
            else if(WiFi.RSSI(1) >= -70) wf_ss_level = 2;
            else wf_ss_level = 3;
            canvas.insert_bitmap_image(POINT<>(net1, 3), sign_wifi_icon[wf_ss_level], 23, 17, 0x1087);
            canvas.insert_text(POINT<>(net1+13, 28), WiFi.SSID(1), 0x1087);
        }
        /// net 2
        if(nets > 2){
            if(WiFi.RSSI(2) >= -30) wf_ss_level = 0;
            else if(WiFi.RSSI(2) >= -67) wf_ss_level = 1;
            else if(WiFi.RSSI(2) >= -70) wf_ss_level = 2;
            else wf_ss_level = 3;
            canvas.insert_bitmap_image(POINT<>(net2, 3), sign_wifi_icon[wf_ss_level], 23, 17, 0x1087);
            canvas.insert_text(POINT<>(net2+13, 28), WiFi.SSID(2), 0x1087);
        }
        /// net 3
        if(nets > 3){
            if(WiFi.RSSI(3) >= -30) wf_ss_level = 0;
            else if(WiFi.RSSI(3) >= -67) wf_ss_level = 1;
            else if(WiFi.RSSI(3) >= -70) wf_ss_level = 2;
            else wf_ss_level = 3;
            canvas.insert_bitmap_image(POINT<>(net3, 3), sign_wifi_icon[wf_ss_level], 23, 17, 0x1087);
            canvas.insert_text(POINT<>(net3+13, 28), WiFi.SSID(3), 0x1087);
        }
        /// net 4
        if(nets > 4 ){
            if(WiFi.RSSI(4) >= -30) wf_ss_level = 0;
            else if(WiFi.RSSI(4) >= -67) wf_ss_level = 1;
            else if(WiFi.RSSI(4) >= -70) wf_ss_level = 2;
            else wf_ss_level = 3;
            canvas.insert_bitmap_image(POINT<>(net4, 3), sign_wifi_icon[wf_ss_level], 23, 17, 0x1087);
            canvas.insert_text(POINT<>(net4+13, 28), WiFi.SSID(4), 0x1087);
        }
        /// rescan button
        canvas.insert_bitmap_image(POINT<>(btn0, 26), rounded_rectangle, 119, 27, 0x3d3b);
        canvas.insert_text(POINT<>(btn0+18, 41), "Re-scan [>]", 0xFFFF);
        /// skip button
        canvas.insert_bitmap_image(POINT<>(btn1, 26), rounded_rectangle, 119, 27, 0xf2b2);
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
                while(0x1);
            }
        }
    }
}
