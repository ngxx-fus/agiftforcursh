#include "Arduino.h"
#include "WiFi.h"

#define LOG false
#define SHOW_AFTER_CHANGED false
#define SYNC_TO_SERIAL false

#include "serial_utils.h"
#include "tft_utils.h"
#include "wifi_utils.h"

void hello(){
    console_push_msg("Hello!!!", "from ngxxfus!");
    console_show();
    delay(1000);
}



void setup_wifi_connection(){
    bool rescan_list = true;
    bool reload_list = true;
    int8_t selected_net = 1;
    while(1){
        if(rescan_list){
            rescan_list = false;
            console_clear();
            wifi_scan(7);
            delay(1000);
            console_clear(); 
            wifi_show_net_list();
            console_push_msg("[O] Rescan");
            console_show(false, selected_net);
        }
        if(reload_list){
            reload_list = false;
            console_show(false, selected_net);
        }
        if( in_range(x_adc_value(), 150, 255) ) {
            selected_net = (selected_net <= ap_list.size())?(selected_net+1):(1);
            reload_list = true;
        }
        if( in_range(x_adc_value(), 0, 90) ) {
            selected_net = (selected_net > 1) ? selected_net : ap_list.size()+1;
            reload_list = true;
        }
        if( sw_value() == 0 ){
            if(selected_net == ap_list.size()+1) {
                rescan_list = true;
                reload_list = true;
                ap_list.pop_back();
                continue;
            }
            if(!wifi_connect(ap_list[selected_net-1])){
                console_clear();
                console_push_msg("Can't connect WiFi!");
                delay(1000);
                rescan_list = true;
                reload_list = true;
                ap_list.pop_back();
                continue;
            }
            break;
        }
    }
}
