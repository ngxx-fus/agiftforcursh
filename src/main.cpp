#include "main.h"

void setup(){
    controller_init();
    serial_init();
    msg2ser("\n\nHello!");
    canvas.initialize(220, 172, 0, 0xFFFF);
}

void loop(){
    msg2ser("H: ", canvas.H());
    msg2ser("W: ", canvas.W());
    canvas.show(true);
    canvas.insert_text(POINT<>(90, 20), "~Hello!#  HAHA", 0xAAAA);
    canvas.insert_text(POINT<>(120, 17), "from ngxxfus :>", 0xAAAA);
    canvas.show();
    delay(1000);
    wifi_setup();
    msg2ser("--nop--");
    while(0x1);
}