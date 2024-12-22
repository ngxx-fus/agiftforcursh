#ifndef WIFI_UTILS_H
#define WIFI_UTILS_H

#include "Arduino.h"
#include "WiFi.h"
#include "console_utils.h"
#include "controller_utils.h"

#include <algorithm>
#include <vector>
using namespace std;

template<class T> using vct = vector<T>;
#ifndef rep
#define rep(i,a,b) for(int i = (a); i <= (b); ++i)
#endif
#ifndef rev
#define rev(i,a,b) for(int i = (b); i >= (a); --i)
#endif

class ACCESS_POINT{
private:
    /// @brief Service Set Identifier (Name of the Wi-Fi)
    String _ssid;

    /// @brief Password of the Wi-Fi 
    String _password;

    /// @brief Received Signal Strength Indicator (unit: dBm)
    uint8_t _rssi;

    /// @brief Encryption
    uint8_t _encryption;

public:

    /// @brief Initializing a ACCESS_POINT object.
    /// @param SSID     Service Set Identifier (Name of the Wi-Fi)
    /// @param PASSWD   Password of the Wi-Fi
    /// @param RSSI     Received Signal Strength Indicator of the Wi-Fi(unit: dBm)
    /// @param ENCRYPT  Encryption of the Wi-Fi
    ACCESS_POINT(String SSID = "", String PASSWD = "", uint8_t RSSI = 254, uint8_t ENCRYPT = true){
        _ssid = SSID, _password = PASSWD, _rssi = RSSI, _encryption = ENCRYPT;
    }

    /// @return SSID of the Wi-Fi
    String& SSID(){return _ssid;}

    /// @return Password of the Wi-Fi
    String& Password(){return _password;}

    /// @return RSSI of the Wi-Fi
    uint8_t& RSSI(){return _rssi;}

    /// @return Encryption of the Wi-Fi (0- Openned AP).
    /// @note Encryption type is defined in <enum wl_enc_type> 
    uint8_t& Encryption(){return _encryption;}

    /// @brief Compare two ACCESS_POINT based on RSSI.
    bool operator <= (ACCESS_POINT o){
        if(RSSI() == o.RSSI()){
            return SSID() < o.SSID();
        }
        return RSSI() < o.RSSI();
    }

    /// @brief Compare two ACCESS_POINT based on RSSI.
    ACCESS_POINT operator = (ACCESS_POINT o){
        SSID() = o.SSID(); RSSI() = o.RSSI();
        Password() = o.Password(); Encryption() = o.Encryption();
        return o;
    }

};

/// @brief access point list
vct<ACCESS_POINT> ap_list;

/// @brief Scan networks
/// @param networks_lim Limit of # networks
/// @return True: found some networks, false: not found any network!
bool wifi_scan(uint8_t networks_lim = 7){
    console_push_msg("Scanning Wi-Fi..."); console_show();
    int8_t networks = WiFi.scanNetworks();
    console_push_msg(
        String("Scaned. Found " ) + 
        String(networks) + 
        String(" nets!")
    ); console_show();
    if( ap_list.empty() )
        ap_list.resize(networks_lim);
    if( networks < ap_list.size())
        ap_list.resize(networks);
    if(networks < 1 || ap_list.empty()) return false;
    uint8_t i = 0;
    for(ACCESS_POINT &ap:ap_list){
        ap.SSID() = WiFi.SSID(i);
        ap.RSSI() = WiFi.RSSI(i);
        ap.Encryption() = WiFi.encryptionType(i);
        if(++i == networks_lim) break;
    }
    sort(ap_list.begin(), ap_list.end(), [](ACCESS_POINT  ap0, ACCESS_POINT  ap1){
        if(ap0.RSSI() == ap1.RSSI()){
            return ap0.SSID() > ap1.SSID();
        }
        return ap0.RSSI() > ap1.RSSI();
    });
    return true;
}

void wifi_show_net_list(){
    console_push_msg("Networks:"); console_show();
    for(int8_t i = 1; i <= ap_list.size(); ++i){
        console_push_msg(
            String(i) + String(" ")
            + String(ap_list[i-1].SSID()) 
            + String((ap_list[i-1].Encryption())?"*":"")
        );
    }
}

bool wifi_connect(ACCESS_POINT ap){
    if(ap.SSID().isEmpty()) {
        return false;
    }

    console_clear();
    /*0*/console_push_msg(String("ESP --> ") + ap.SSID());
    console_show();

    if(ap.Encryption() == 0){
        WiFi.begin(ap.SSID(), "");
        
        for(int8_t i = 5; i >= 0; --i){
            if(WiFi.status() == WL_CONNECTED){
                console_push_msg(String("Connected!"));
                console_show();
            }
            if(i ==  0) return false;
            console_push_msg(String("Failed! Re-try...!"));
            console_show();
            delay(1000);
        }
    }

    /*1*/console_push_msg("Enter password for");
    /*2*/console_push_msg(
        String(" <") 
        + ap.SSID()
        + String("> :")
    );
    console_show();

    bool reload_view = true;
    char selected_char = 32;
    int8_t selected_line = 4;

    /*3*/console_push_msg(ap.Password());
    /*4*/console_push_msg(
        String("[>]  Enter [") 
        + String(selected_char) 
        + String("]")
    ); 
    /*5*/console_push_msg("[<]  Delete");
    /*6*/console_push_msg("[v]  Connect ");
    /*7*/console_push_msg("[x]  Cancel  ");
    console_show();
    while(1){
        if(reload_view){
            reload_view = false;
            /*3*/console_set_line(3, ap.Password());
            /*4*/console_set_line( 4,
                String("[>]  Enter [") 
                + String(selected_char) 
                + String("]")
            ); 
            /*5*/console_set_line(5, "[<]  Delete");
            /*6*/console_set_line(6, "[v]  Connect ");
            /*7*/console_set_line(7, "[x]  Cancel  ");
            line_ptr = 7;
            console_show(false, selected_line);
        }

        if(selected_line == 4){
            if( in_range(y_adc_value(), 0, 30) ) {
                (selected_char>32)
                ?(--selected_char):(selected_char=126);
                reload_view = true;
            }
            if( in_range(y_adc_value(), 230, 255) ) {
                (selected_char<126)
                ?(++selected_char):(selected_char=32);
                reload_view = true;
            }
        }
        if( in_range(x_adc_value(), 230, 255) ){
            (selected_line < 7)
            ? (++selected_line)
            : (selected_line = 4);
            reload_view = true;
        }
        if( in_range(x_adc_value(), 0, 30) ) {
            (selected_line > 4)
            ? (--selected_line)
            : (selected_line = 7);
            reload_view = true;
        }
        if( sw_value() == 0){
            switch (selected_line)
            {
            case 4:
                ap.Password() += selected_char; break;
            case 5:
                ap.Password().remove(ap.Password().length()-1); break;
            case 6:
                WiFi.begin(ap.SSID(), ap.Password());
                for(int8_t i = 5; i >= 0; --i){
                    delay(1000);
                    if(WiFi.status() == WL_CONNECTED){
                        console_push_msg(String("Connected!"));
                        console_show();
                        return true;
                    }
                    if(i ==  0) return false;
                    console_push_msg(String("Failed! Re-try...!"));
                    console_show();
                }
                return false;
            case 7:
                return false;
            }
            reload_view = true;
        }
    }
    return true;
}

/// @brief Initializing wifi with STA mode.
void wifi_init(){
    WiFi.mode(WIFI_STA);
}

#endif
