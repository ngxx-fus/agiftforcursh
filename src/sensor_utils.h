#ifndef SENSOR_UTILS
#define SENSOR_UTILS



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



#endif