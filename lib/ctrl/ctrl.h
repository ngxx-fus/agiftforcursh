#ifndef __CTRL_H__
#define __CTRL_H__

#include "../../include/global.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern flag64_t btnStatus;

enum CTRL_ENUM_ORDER_BITMASK{
    NEXT_IMG = 0x1,
};

typedef int8_t      pin_t;
typedef uint16_t    xy_t;
typedef uint64_t    flag64_t;
typedef void (*intServiceFn_t)(void*);

/// @brief Simple User Interactive (LEDs and Buttons)
typedef struct simpleUserInteractive{
    flag64_t buttonMask;        /// GPIO button mask
    flag64_t ledMask;           /// GPIO LED mask (output only)
}simpUI;

/// @brief Create new Simple User Interactive
/// @param sui The address of variable that simpUI pointer is stored
/// @param btnMask Button GPIO mask (64-bit mask), By default, all pin are enable negedge-interrupt.
/// @param ledMask LED GPIO mask (64-bit mask)
void createNewSUI(simpUI ** sui, flag64_t btnMask, flag64_t ledMask);

/// @brief Attach Interrupt Service Function to a GPIO
/// @param sui Simple User Interactive group
/// @param pin Pin to attach 
/// @param isf Interrupt Service Function (get (void*) and return void)
void attachISF(simpUI * sui, pin_t pin,  intServiceFn_t isf);

void ledBlink(flag64_t ledMask, millisecTime_t msTimeDelay, uint32_t num);

#endif