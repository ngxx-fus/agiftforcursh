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
    if(is_connected_to_wifi){
        #if LOG == true
            log2ser( "Wi-Fi connection!");
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
    do Firebase.begin(&config, &auth);
    while(!Firebase.ready());
    #if LOG == true
        msg2ser( "Firebase: ", "Connected to Firebase!");
    #endif
    Firebase.reconnectWiFi(true);
}

#endif 