#include <Arduino.h>
#include <algorithm>
using namespace std;
#define  elif else if

#if CONTROLLER == true
    #include "controller_utils.h"
#endif

#if USB_SERIAL == true
    #include "serial_utils.h"
#endif

#include "tft_utils.h"
#include "images.h"

#if SENSORS == true
    #include "sensor_utils.h"
#endif

#if WIFI_CONNECTION == true
    #include "wifi_utils.h"
#endif

