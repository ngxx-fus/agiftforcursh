#include <Arduino.h>
#include "serial_utils.h"
#include "tft_utils.h"
#include "images.h"

/// @brief The canvas object
CANVAS<uint16_t> canvas;

void setup(){
    serial_init();
    msg2ser("\n\nHello!");
    canvas.initialize(220, 172, 0, 0xFFFF);
}

void wifi_setup(){
    uint16_t text0 = 30, wifi_icon = 70, btn0 = 130, btn1 = btn0 + 30;
    canvas.restore();
    canvas.clear(true);

    /// Show skip or scan question?

    canvas.insert_rectangle(POINT<>(text0, 1), 170, 27, 0x0, true, 0xf7be);
    canvas.insert_text(POINT<>(text0+18, 30), "Set-up Wi-Fi?", 0x2945);

    canvas.insert_bitmap_image(POINT<>(wifi_icon, 50), wifi_icon70x53, 70, 53, 0x0964);

    canvas.insert_bitmap_image(POINT<>(btn0, 26), rounded_rectangle, 119, 27, 0x3d3b);
    canvas.insert_text(POINT<>(btn0+18, 56), "Scan [>]", 0xFFFF);

    canvas.insert_bitmap_image(POINT<>(btn1, 26), rounded_rectangle, 119, 27, 0xf2b2);
    canvas.insert_text(POINT<>(btn1+18, 56), "Skip [>]", 0xFFFF);

    canvas.show(true);

}

void loop(){
    tft.drawCircle(44, 90, 5, 0xEEEE);

    msg2ser("H: ", canvas.H());
    msg2ser("W: ", canvas.W());
    canvas.show(true);
    canvas.insert_text(POINT<>(90, 20), "~Hello!#  HAHA", 0xAAAA);
    canvas.show();
    delay(1000);
    wifi_setup();
    msg2ser("--nop--");
    while(0x1);
}