#define WIFI_CONNECTION false
#define SENSORS true
#define CONTROLLER true
#define USB_SERIAL true
#define LOG true

#include "main.h"

void setup(){
    controller_init();
    serial_init();
    sensor_init();
    msg2ser("\n\nHello!");
    canvas_init();
    #if WIFI_CONNECTION == true
        delay(1000);
        wifi_setup();
    #endif
    msg2ser("normal mode");
}



void loop(){
    update_humid_temp();
    delay(4000);
    // msg2ser("--nop--");
    // while(0x1);
}





















