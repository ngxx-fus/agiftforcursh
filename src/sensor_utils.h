#ifndef SENSOR_UTILS
#define SENSOR_UTILS

#include "tft_utils.h"
#ifndef TFT_UTILS_H
    #pragma error("Please include tft_utils.h (bcz it \
    contain ```canvas``` object for comunication with \
    tft screen)")
#endif

#ifndef DHT_PIN
#define DHT_PIN 16
#endif

#ifndef DHT_TYPE
#define DHT_TYPE DHT11
#endif


#include <DHT.h>

DHT dht(DHT_PIN, DHT_TYPE);

/// @brief read temp from DHT sensor
/// @param measure_time delay before measure 
/// (at lease 2s between 2 times)
/// @return (float) Temperature
/// @note return value: 0..50
inline float read_temp(uint16_t measure_time = 2000){
    delay(measure_time); 
    float res = dht.readTemperature(); 
    return isnan(res)?-1:res;
}

/// @brief read humid from DHT sensor
/// @param measure_time delay before measure 
/// (at lease 2s between 2 times)
/// @return (float) humidity
/// @note return value: 0..100
inline float read_humid(uint16_t measure_time = 2000){
    delay(measure_time); 
    float res = dht.readHumidity(); 
    return isnan(res)?-1:res;
}

/// @brief Initializing DHT11 sensor
void sensor_init(){

    msg2ser("call\tsensor_init:");
    msg2ser("\t", "DHT_PIN: ", DHT_PIN);
    msg2ser("\t", "DHT_TYPE: ", DHT_TYPE);

    dht.begin();
}

bool run_show_envinfo = true;

/// @brief Show Humid and temp ultil show_envinfo==false
/// @note Maximum of history_size and col_d (column distance) is 15px and 10px
void run_env_info(uint32_t const history_size = 10, uint16_t col_d = 10){

    msg2ser("call\t", "run_env_info");

    uint8_t title0 = 2, humid_row = title0 + 40, temp_row = humid_row + 22,
            btn0 = 185, chart_row = temp_row + 40;
    static float humid[20], temp[20];
    bool _1st_time = true;
    
    while(run_show_envinfo){
        if( !_1st_time && millis()%4000 ) continue;
        if(_1st_time) _1st_time = false;
        /// clear screen
        canvas.clear();
        /// show title
        canvas.insert_rectangle(POINT<>(title0-1, 1), 170, 35, 0x18c3, true, 0x18c3);
        canvas.insert_bitmap_image(POINT<>(title0+1, 2), _32x32_env_icon, 32, 32, 0x34eb);
        canvas.insert_text(POINT<>(title0+22, 35), "Environment Info", 0xdfdd);
        /// show next mode button
        canvas.insert_bitmap_image(POINT<>(btn0, 15), _64x27_rounded_rectangle, 64, 27, 0x6b4d);
        canvas.insert_text(POINT<>(btn0+18, 25), "Mode", 0xFFFF);
        canvas.insert_bitmap_image(POINT<>(btn0, 93), _64x27_rounded_rectangle, 64, 27, 0x6b4d);
        canvas.insert_text(POINT<>(btn0+18, 108), "Next", 0xFFFF);
        /// show temp and humid icon
        canvas.insert_bitmap_image(POINT<>(humid_row, 8), _13x18_humid_icon, 13, 18, 0x3675);
        canvas.insert_bitmap_image(POINT<>(temp_row, 8), _13x18_temp_icon, 13, 18, 0xdaeb);
        /// show x-axis, y-axis
        canvas.insert_line(POINT<>(chart_row, 163), POINT<>(chart_row+75, 163), 0x10c3);
        canvas.insert_line(POINT<>(chart_row, 5), POINT<>(chart_row+75, 5), 0x10c3);
        canvas.insert_line(POINT<>(chart_row+74, 5), POINT<>(chart_row+74, 163), 0x10c3);
        /// shift-left chart
        rept(uint16_t, i, 0, history_size-1) humid[i] = humid[i+1], temp[i] = temp[i+1];
        /// read temp
        humid[history_size-1] = read_humid(0), temp[history_size-1] = read_temp(0);
        msg2ser("Humid: ", humid[history_size-1]); msg2ser("Temp: ", temp[history_size-1]);
        /// show temp, humid;
        canvas.insert_text(POINT<>(humid_row+15, 29), String("Humid: ")+String(humid[history_size-1]), 0x3675);
        canvas.insert_text(POINT<>(temp_row+15, 29), String("Temp: ")+String(temp[history_size-1]), 0xdaeb);
        /// show chart
        rept(uint16_t, i, 0, history_size-1){
            uint16_t h = max(0.0f, humid[i]) * 75 / 100.0;
            canvas.insert_line(POINT<>(chart_row+75, 8 + i*col_d), POINT<>(chart_row+75-h, 8 + i*col_d), 0x3675);
            canvas.insert_line(POINT<>(chart_row+75, 9 + i*col_d), POINT<>(chart_row+75-h, 9 + i*col_d), 0x3675);
            canvas.insert_line(POINT<>(chart_row+75, 10 + i*col_d), POINT<>(chart_row+75-h, 10 + i*col_d), 0x3675);
            h = max(0.0f, temp[i]) * 75 / 53.0;
            canvas.insert_line(POINT<>(chart_row+75, 12 + i*col_d), POINT<>(chart_row+75-h, 12 + i*col_d), 0xdaeb);
            canvas.insert_line(POINT<>(chart_row+75, 13 + i*col_d), POINT<>(chart_row+75-h, 13 + i*col_d), 0xdaeb);
            canvas.insert_line(POINT<>(chart_row+75, 14 + i*col_d), POINT<>(chart_row+75-h, 14 + i*col_d), 0xdaeb);
        }
        /// show legend (max value)
        canvas.insert_text(POINT<>(chart_row+3, 5), "100%", 0x3675);
        canvas.insert_text(POINT<>(chart_row+3, 123), "50oC", 0xdaeb);
        /// show changed
        canvas.show();
    }
}
#endif