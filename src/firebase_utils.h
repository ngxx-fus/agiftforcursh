#ifndef FIREBASE_UTILS_H
#define FIREBASE_UTILS_H

#include <Arduino.h>
#include "WiFi.h"

#include "serial_utils.h"

const char  DATABASE_URL[] =    "https://agiftforcrush-default-rtdb.firebaseio.com/";
const char  API_KEY[]      =    "AIzaSyD2KF4BJw4Ye3aDAzs8P4K9Bj7D_dOevWM";
const char EMAIL[]         =    "prototype-account@agiftforcrush.iam.gserviceaccount.com";
const char PASSWORD[]      =    "prototype@pw3204";

#include "FirebaseESP32.h"

FirebaseData firebaseData;
FirebaseAuth auth;
FirebaseConfig config;

//-------------- Firebase --------------//
bool is_connected_to_wifi(){
    if(WiFi.status() != WL_CONNECTED){
        #if CONTROLLER == true
            controller::iled_blinky(10);
        #endif
        #if LOG == true
            log2ser( "Firebase: ", "No Wi-Fi connection!");
        #endif
        return false;
    }
    return true;
}

void firebase_init() {
    #if LOG == true
        call("firebase_init");
    #endif
    /// try to connect manufacture wifi
    String  mSSID = "hg.xnb",
            mPW   = "nGXXFUS@3204";

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
        if(WiFi.status() == WL_CONNECTED) {
            #if BASIC_IO == true
                basic_io::led1_blinky(2);
            #endif
            #if LOG == true
                log2ser("Succesful!");
            #endif
            goto FIREBASE_CONNECT;
        }
    }

    #if LOG == true
        log2ser("Failed!");
        log2ser("Scanning other Wi-Fis...");
    #endif

    FIREBASE_CONNECT:

    if(!is_connected_to_wifi()){
        #if LOG == true
            log2ser( "No Wi-Fi connection!");
        #endif
        return;
    }
    #if LOG == true
        log2ser( "Firebase: ", "Connecting to Firebase...");
    #endif
    config.api_key = (String) API_KEY;
    config.database_url = (String) DATABASE_URL;
    auth.user.email = String(EMAIL);
    auth.user.password = (String) PASSWORD;
    firebaseData.setBSSLBufferSize(512, 512);
    firebaseData.setResponseSize(512);
    firebaseData.setCert(NULL);
    // firebaseData.setReadTimeout(firebaseData, 5000);
    for(uint8_t i = 0; i < 5; ++i) {
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
        if(Firebase.ready()) break;
    }
    Firebase.reconnectWiFi(true);
    #if BASIC_IO == true
        basic_io::led1_blinky(2);
    #endif
    #if LOG == true
        msg2ser(  "Connected to Firebase!");
    #endif
}

#endif 