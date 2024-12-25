#ifndef SENSOR_UTILS
#define SENSOR_UTILS

#include "tft_utils.h"
#ifndef TFT_UTILS_H
#pragma error("Please include tft_utils.h (bcz it \
contain ```canvas``` object for comunication with \
tft screen)")
#endif

#define DHT_PIN 16
#define DHT_TYPE DHT11
#include <DHT.h>
DHT dht(DHT_PIN, DHT_TYPE);

inline float read_temp(uint16_t measure_time = 2000){
    delay(measure_time); 
    float res = dht.readTemperature(); 
    return isnan(res)?-1:res;
}

inline float read_humid(uint16_t measure_time = 2000){
    delay(measure_time); 
    float res = dht.readHumidity(); 
    return isnan(res)?-1:res;
}

void sensor_init(){
    dht.begin();
}

void update_humid_temp(){
    uint8_t title0 = 2, humid_row = title0 + 40, temp_row = humid_row + 22,
            btn0 = 185, chart_row = temp_row + 35;
    static bool _1st_call = false;
    static float humid[10], temp[10];
    /// change in screen
    canvas.restore();
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
    canvas.insert_line(POINT<>(chart_row, 5), POINT<>(chart_row+75, 5), 0x10c3);
    canvas.insert_line(POINT<>(chart_row+74, 5), POINT<>(chart_row+74, 163), 0x10c3);
    canvas.show(true);
    /// shift-left chart
    rept(uint16_t, i, 0, 8) humid[i] = humid[i+1], temp[i] = temp[i+1];
    /// read temp
    humid[9] = read_humid(0), temp[9] = read_temp(0);
    msg2ser("Humid: ", humid[9]); msg2ser("Temp: ", temp[9]);
    /// show temp, humid;
    canvas.insert_text(POINT<>(humid_row+15, 29), String("Humid: ")+String(humid[9]), 0x3675);
    canvas.insert_text(POINT<>(temp_row+15, 29), String("Humid: ")+String(temp[9]), 0xdaeb);
    /// show chart
    rept(uint16_t, i, 0, 9){
        uint16_t h = max(0.0f, humid[i]) * 75 / 100.0;
        canvas.insert_line(POINT<>(chart_row+75, 8 + i*15), POINT<>(chart_row+75-h, 8 + i*15), 0x3675);
        canvas.insert_line(POINT<>(chart_row+75, 9 + i*15), POINT<>(chart_row+75-h, 9 + i*15), 0x3675);
        canvas.insert_line(POINT<>(chart_row+75, 10 + i*15), POINT<>(chart_row+75-h, 10 + i*15), 0x3675);
        h = max(0.0f, temp[i]) * 75 / 100.0;
        canvas.insert_line(POINT<>(chart_row+75, 12 + i*15), POINT<>(chart_row+75-h, 12 + i*15), 0xdaeb);
        canvas.insert_line(POINT<>(chart_row+75, 13 + i*15), POINT<>(chart_row+75-h, 13 + i*15), 0xdaeb);
        canvas.insert_line(POINT<>(chart_row+75, 14 + i*15), POINT<>(chart_row+75-h, 14 + i*15), 0xdaeb);
    }
    /// show changed
    canvas.show();
}

#endif