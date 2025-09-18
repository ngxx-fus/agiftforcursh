#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>

/// PIN CONFIG ////////////////////////////////////////////////////////////////////////////////////

#define DHT_TYPE                DHT11
#define DHT_PIN                 16

#define SDCARD_SPI_CS_PIN       5

#define BTN0_PIN                32 
#define BTN1_PIN                34
#define BTN2_PIN                35         
#define BTN3_PIN                17
#define BTN4_PIN                33

#define LED_YELLOW_PIN          26
#define LED_WHITE_PIN           27

/// HELPERS ///////////////////////////////////////////////////////////////////////////////////////

#include "espidf-helper.h"

enum SYSTEM_STATUS_FLAG{
    SYS_RUNNING = 0x0   ,
};

extern flag64_t systemStatus;

/// RETURN ////////////////////////////////////////////////////////////////////////////////////////

typedef uint8_t defReturn_t;

enum DEFAULT_RETURN_STATUS{
    STATUS_OKE = 0  ,   /// Status oke, no error
    STATUS_FAILED       /// Status failed, unknown error
};

#endif 