#ifndef SERIAL_UTILS_H
#define SERIAL_UTILS_H

#include "Arduino.h"

#ifndef LOG
    #define LOG false
#endif

template<class Tmsg>
void msg2ser(Tmsg msg){
    #if LOG == true
        Serial.println(String(msg));
    #endif
}

template<class Tmsg, class... Tmsgs>
void msg2ser(Tmsg msg, Tmsgs... msgs){
    #if LOG == true
        Serial.print(msg);
        msg2ser(msgs...);
    #endif
}

void serial_init(){
    #if LOG == true
        Serial.begin(115200);
        msg2ser("\n\nHello!");
    #endif
}


#endif