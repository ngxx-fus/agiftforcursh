#include "serial_utils.h"
#include "tft_utils.h"

/// @brief The canvas object
CANVAS<uint16_t> canvas;

void setup(){
    serial_init();
    msg2ser("\n\n\nHello!\n\n\n");
    canvas.resize(220, 172, 0x0);
    tft.begin();
}

void loop(){
    tft.drawCircle(44, 44, 5, 0x1245);

    msg2ser("H: ", canvas.H());
    msg2ser("W: ", canvas.W());
    canvas.show();
    canvas.insert_text(POINT<>(60, 4), "Hehehe", 0xFFFF);
    canvas.insert_text(POINT<>(60, 14), "Hehehe", 0xFFFF);
    canvas.show();
    msg2ser("_nop()");
    while(0x1);
}

