#ifndef _FIREBASE_THINGSPEAK_DEF_H_
#define _FIREBASE_THINGSPEAK_DEF_H_

#include <Arduino.h>
#include "WiFi.h"


// Định nghĩa các macro để kết nối tới Firebase
#define DATABASE_URL "https://lab3-itfa-default-rtdb.firebaseio.com/"
#define API_KEY "AIzaSyBMq4aT1Jx6nICW4yuYtTSIU9QSzIa1vmk"
#define EMAIL "ngxxfus@lab3-itfa.iam.gserviceaccount.com"
#define PASSWORD "nGXXFUS@3204"

#include "serial_utils.h"
#include "controller_utils.h"
#include "FirebaseESP32.h"

FirebaseData firebaseData;
FirebaseAuth auth;
FirebaseConfig config;

//-------------- Firebase --------------//
bool is_connected_to_wifi(){
    if(WiFi.status() != WL_CONNECTED){
        controller::iled_blinky(10);
        msg2ser("\t", "Firebase: ", "No Wi-Fi connection!");
        return false;
    }
    return true;
}

void firebase_init() {
    msg2ser("\t", "Firebase: ", "Connecting to Firebase...");
    config.api_key = (String) API_KEY;
    config.database_url = (String) DATABASE_URL;
    auth.user.email = String(EMAIL);
    auth.user.password = (String) PASSWORD;
    do Firebase.begin(&config, &auth);
    while(!Firebase.ready());
    msg2ser("\t", "Firebase: ", "Connected to Firebase!");
    Firebase.reconnectWiFi(true);
}

#endif 