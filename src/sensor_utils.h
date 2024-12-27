#ifndef SENSOR_UTILS
#define SENSOR_UTILS

#define sensors_color_0 0xffff          /// screen background
#define sensors_color_1 0x18c3          /// title bar background
#define sensors_color_2 0xdfdd          /// title bar text
#define sensors_color_3 0x34eb          /// title bar icon
#define sensors_color_4 0x6b4d          /// left button background
#define sensors_color_5 0xffff          /// left button text
#define sensors_color_6 0x6b4d          /// left button background
#define sensors_color_7 0xffff          /// left button text
#define sensors_color_8  0x3675         /// humid icon
#define sensors_color_9  0x3675         /// humid text ["label":"value"]
#define sensors_color_10 0xdaeb         /// temp icon
#define sensors_color_11 0xdaeb         /// temp text  ["label":"value"]
#define sensors_color_12 0x3675         /// humid legend
#define sensors_color_13 0xdaeb         /// temp legend
#define sensors_color_14 0x10c3         /// left x-axis (vertical axis)  
#define sensors_color_15 0x10c3         /// right x-axis (vertical axis)
#define sensors_color_16 0x10c3         /// bottom y-axis (horizontal axis)
#define sensors_color_18 0x3675         /// humid
#define sensors_color_19 0xdaeb         /// temp chart
#define chart_padding    0x4            /// left x-axis <---d---> 1st chart
#define sampling_interval 2000          /// dht11 required at leaset 2s
#define chart_interval   10000           /// dht11 required at leaset 2s

#include "tft_utils.h"
#if SHOW_AUTHOR_MESSAGE == true
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
namespace sensors{

    unsigned long last_read = 0;
    unsigned long last_update_chart = 0;

    /// @brief read temp from DHT sensor
    /// @param measure_time delay before measure 
    /// (at lease 2s between 2 times)
    /// @return (float) Temperature
    /// @note return value: 0..50
    inline float read_temp(uint16_t measure_time = 2000){
        sensors::last_read = millis();
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
        sensors::last_read = millis();
        delay(measure_time); 
        float res = dht.readHumidity(); 
        return isnan(res)?-1:res;
    }
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
/// @param history_size number of recorded values (max:15)
/// @param column_distance distance between two pair of humid-temp chart
/// @note - You can calc the ```col_d``` by quite right eq: 
/// ```screen_width = col_d * (history_size-1) + 7 * history_size + 4```.
/// 4 for padding 2 pixels at the left and right screen edge.
/// 7 for each pair of humid-temp chart (3 for humid and temp, and 1 is distance 
/// between them).
/// @note - Maximum of history_size and col_d (column distance) is 15px and 10px.
void run_env_info(uint16_t const history_size = 15, uint16_t const column_distance = 10){

    msg2ser("call\t", "run_env_info");

    uint8_t title0 = 2, humid_row = title0 + 40, temp_row = humid_row + 22,
            btn0 = 185, chart_row = temp_row + 40;
    static float humid[17], temp[17], latest_humid, latest_temp;

    uint16_t n_lines = history_size, col_d  = column_distance;

    while(run_show_envinfo){
        
        /// for zoom :v
        if( x_adc_value() < 20 ) n_lines = (n_lines>3)?(n_lines-1):(n_lines);
        else if( x_adc_value() > 200 ) n_lines = (n_lines < history_size)?(n_lines+1):(n_lines);
        /// re-calc col_d
        if(n_lines < history_size)
            col_d = 150 / n_lines;
        else
            col_d = column_distance;

        /// 4sec between two measure
        if(  millis() - sensors::last_read >= sampling_interval ){
            /// read temp
            latest_humid = sensors::read_humid(0),
            latest_temp  = sensors::read_temp(0);
        }

        /// 4sec between two measure
        if(  millis() - sensors::last_update_chart >= chart_interval ){
            sensors::last_update_chart = millis();
            /// sync to usb serial
            msg2ser("Humid: ", latest_humid); 
            msg2ser("Temp:  ", latest_temp);
            /// shift-left chart
            rept(uint16_t, i, 0, 15)
                humid[15-i+1] = humid[15-i], temp[15-i+1] = temp[15-i];
            /// add latest value
            *humid = latest_humid, *temp  = latest_temp;
                
        }

        /// clear screen
        canvas.refill(sensors_color_0);
        /// show title
        canvas.insert_rectangle(POINT<>(title0-1, 1), 170, 35, 0x18c3, true, sensors_color_1);
        canvas.insert_bitmap_image(POINT<>(title0+1, 2), _32x32_env_icon, 32, 32, sensors_color_3);
        canvas.insert_text(POINT<>(title0+22, 35), "Environment Info", sensors_color_2);
        /// show next mode button
        show_2button_on_1line( btn0, "Mode", 25, "Exit", 111, sensors_color_4, sensors_color_6, sensors_color_5, sensors_color_7 );
        /// show temp and humid icon
        canvas.insert_bitmap_image(POINT<>(humid_row, 8), _13x18_humid_icon, 13, 18, sensors_color_8);
        canvas.insert_bitmap_image(POINT<>(temp_row, 8), _13x18_temp_icon, 13, 18, sensors_color_10);
        /// show x-axis, y-axis
        canvas.insert_line(POINT<>(chart_row, 5), POINT<>(chart_row+75, 5), sensors_color_14);
        canvas.insert_line(POINT<>(chart_row, 163), POINT<>(chart_row+75, 163), sensors_color_15);
        canvas.insert_line(POINT<>(chart_row+74, 5), POINT<>(chart_row+74, 163), sensors_color_16);
        /// show temp, humid;
        canvas.insert_text(POINT<>(humid_row+15, 29), String("Humid: ")+String(latest_humid), sensors_color_9);
        canvas.insert_text(POINT<>(temp_row+15, 29), String("Temp: ")+String(latest_temp), sensors_color_11);
        
        /// show chart
        uint16_t h;
        rept(uint16_t, i, 0, n_lines-1){
            
            h = max(0.0f, humid[n_lines-1-i]) * 75 / 100.0;
            rept(uint16_t, d, 6 + chart_padding, 6 + chart_padding + 2)
                canvas.insert_line(
                    POINT<>(chart_row+75,   d + i*col_d), 
                    POINT<>(chart_row+75-h, d + i*col_d), 
                    sensors_color_18
                );
            
            h = max(0.0f, temp[n_lines-1-i]) * 75 / 53.0;
            rept(uint16_t, d, 6 + chart_padding + 4, 6 + chart_padding + 6)
                canvas.insert_line(
                    POINT<>(chart_row+75,   d + i*col_d), 
                    POINT<>(chart_row+75-h, d + i*col_d), 
                    sensors_color_19
                );
        }
        /// show legend (max value)
        canvas.insert_text(POINT<>(chart_row+3, 5), "100%", 0x3675);
        canvas.insert_text(POINT<>(chart_row+3, 123), "50oC", 0xdaeb);
        /// show changed
        canvas.show();
        /// wait for the joystick released
        while( x_adc_value() > 200 || x_adc_value() < 30);
    }
}
#endif