#ifndef __CTRL_H__
#define __CTRL_H__

#include "../../include/global.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern flag64_t btnStatus;

void buttonAndLEDInit();
void ledBlink(flag64_t ledPinFlag, millisecTime_t msTimeDelay, uint32_t num);

#endif