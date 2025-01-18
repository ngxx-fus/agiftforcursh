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
#define sensors_color_8  0x14f2         /// humid icon
#define sensors_color_9  0x14f2         /// humid text ["label":"value"]
#define sensors_color_10 0xcb4d         /// temp icon
#define sensors_color_11 0xcb4d         /// temp text  ["label":"value"]
#define sensors_color_12 0x14f2         /// humid legend
#define sensors_color_13 0xcb4d         /// temp legend
#define sensors_color_14 0x10c3         /// left x-axis (vertical axis)  
#define sensors_color_15 0x10c3         /// right x-axis (vertical axis)
#define sensors_color_16 0x10c3         /// bottom y-axis (horizontal axis)
#define sensors_color_18 0x14f2         /// humid chart
#define sensors_color_19 0xcb4d         /// temp chart
#define sensors_color_20 0xc618         /// grid line
#define sensors_color_21 0xfed3         /// selected box
#define chart_padding    0x4            /// left x-axis <---d---> 1st chart
#define max_history_size 25             /// N/A
#define hold_time_thres  500            /// N/A
#define hold_for_select  2000U          /// N/A
#define chart_interval   10000          /// dht11 required at leaset 2s
#define sampling_interval 2000          /// dht11 required at leaset 2s

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
#include "general_utils.h"

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
    #if LOG == true
        call("sensor_init:");
        log2ser("\t", "DHT_PIN: ", DHT_PIN);
        log2ser("\t", "DHT_TYPE: ", DHT_TYPE);
    #endif
    dht.begin();
}

/// @brief Show Humid and temp ultil show_envinfo==false
/// @param history_size number of recorded values (max: max_history_size)
/// @param column_distance distance between two pair of humid-temp chart
/// @note - You can calc the ```col_d``` by quite right eq: 
/// ```col_d ~ screen_width / history_size```.
/// 4 for padding 2 pixels at the left and right screen edge.
/// 7 for each pair of humid-temp chart (3 for humid and temp, and 1 is distance 
/// between them).
/// @note - Maximum of history_size and col_d (column distance) is 15px and 10px.
void run_env_info(uint16_t const history_size = 15, uint16_t const column_distance = 10){

    // #if LOG == true
    //     call( "run_env_info");
    // #endif

    // uint8_t title0 = 2, humid_row = title0 + 37, temp_row = humid_row + 20,
    //         btn0 = 185, chart_row = temp_row + 40;
    // float humid[max_history_size], temp[max_history_size], latest_humid, latest_temp;
    // uint16_t start_pos = 0, end_pos;
    // uint16_t n_lines = history_size, col_d  = column_distance;
    // uint32_t last_t = 0;
    // uint16_t sel = 0, selectedBox_W = 68, selectedBox_H = 31;
    // bool selected = false;

    // while(0x1){


    //     // /// for Bar spacing :v
    //     // if( x_adc_value() < 30 ) {
    //     //     msg2ser("\t", "Bar spacing: +1");
    //     //     n_lines = (n_lines>3)?(n_lines-1):(n_lines);
    //     //     last_t = millis(); while( x_adc_value() < 30 && millis() - last_t < hold_time_thres);
    //     // }else 
    //     //     if ( x_adc_value() > 190 ) {
    //     //         msg2ser("\t", "Bar spacing: -1");
    //     //         n_lines = (n_lines < history_size)?(n_lines+1):(n_lines);
    //     //         last_t = millis(); while( x_adc_value() > 190 && millis() - last_t < hold_time_thres);
    //     //     }
    //     // /// for left-right shift
    //     // if( y_adc_value() < 30 ) {
    //     //     msg2ser("\t", "Shifted: +1 left");
    //     //     start_pos = (start_pos < max_history_size - 1 - n_lines-1) ? (start_pos+1) : start_pos;
    //     //     last_t = millis(); while( y_adc_value() < 30 && millis() - last_t < hold_time_thres);
    //     // }else 
    //     //     if ( y_adc_value() > 190 ) {
    //     //         msg2ser("\t", "Shifted: +1 right");
    //     //         start_pos = (start_pos > 0) ? (start_pos-1) : start_pos;
    //     //         last_t = millis(); while( y_adc_value() > 190 && millis() - last_t < hold_time_thres);
    //     //     }
        
    //     /// re-calc col_d
    //     if(n_lines < history_size)
    //         col_d = 150 / n_lines;
    //     else
    //         col_d = column_distance;

    //     /// 4sec between two measure
    //     if( t_since(sensors::last_read) >= sampling_interval ){
    //         /// read temp
    //         // controller::iled_blinky(1); 
    //         latest_humid = sensors::read_humid(0),
    //         latest_temp  = sensors::read_temp(0);
    //     }

    //     /// 4sec between two measure
    //     if(  t_since(sensors::last_update_chart) >= chart_interval ){
    //         sensors::last_update_chart = millis();
    //         /// sync to usb serial
    //         msg2ser("\t", "Humid: ", latest_humid); 
    //         msg2ser("\t", "Temp:  ", latest_temp);
    //         /// shift-left chart
    //         rept(uint16_t, i, 0, 15)
    //             humid[15-i+1] = humid[15-i], temp[15-i+1] = temp[15-i];
    //         /// add latest value
    //         *humid = latest_humid, *temp  = latest_temp;
                
    //     }

    //     /// for waring user that they are at the leftmost part of the chart.
    //     if(start_pos == max_history_size - 1 - n_lines-1) 
    //         canvas.refill(0xfe9a);
    //     else
    //     /// clear screen
    //         canvas.refill(sensors_color_0);
        

    //     /// show title
    //     canvas.insert_rectangle(POINT<>(title0-1, 1), 170, 35, 0x18c3, true, sensors_color_1);
    //     canvas.insert_bitmap_image(POINT<>(title0+1, 2), _32x32_env_icon, 32, 32, sensors_color_3);
    //     canvas.insert_text(POINT<>(title0+22, 35), "Environment Info", sensors_color_2);
    //     /// show next mode button
    //     show_2button_on_1line( btn0, "Mode", 25, "Exit", 111, sensors_color_4, sensors_color_6, sensors_color_5, sensors_color_7 );
    //     /// show temp and humid icon
    //     canvas.insert_bitmap_image(POINT<>(humid_row, 8), _13x18_humid_icon, 13, 18, sensors_color_8);
    //     canvas.insert_bitmap_image(POINT<>(temp_row, 8), _13x18_temp_icon, 13, 18, sensors_color_10);
    //     /// show 100% grid
    //     canvas.insert_line(POINT<>(chart_row, 5), POINT<>(chart_row, 163), sensors_color_20);
    //     /// show 75% grid
    //     canvas.insert_line(POINT<>(chart_row+18, 5), POINT<>(chart_row+18, 163), sensors_color_20);
    //     /// show 50% grid
    //     canvas.insert_line(POINT<>(chart_row+37, 5), POINT<>(chart_row+37, 163), sensors_color_20);
    //     /// show 25% grid
    //     canvas.insert_line(POINT<>(chart_row+56, 5), POINT<>(chart_row+56, 163), sensors_color_20);
    //     /// show y-axis
    //     canvas.insert_line(POINT<>(chart_row+74, 5), POINT<>(chart_row+74, 163), sensors_color_16);
    //     /// show x-axis
    //     canvas.insert_line(POINT<>(chart_row, 5), POINT<>(chart_row+74, 5), sensors_color_14);
    //     canvas.insert_line(POINT<>(chart_row, 163), POINT<>(chart_row+74, 163), sensors_color_15);
    //     /// show temp, humid;
    //     canvas.insert_text(POINT<>(humid_row+15, 29), String("Humid: ")+String(latest_humid)+String(" oC"), sensors_color_9);
    //     canvas.insert_text(POINT<>(temp_row+15, 29), String("Temp:   ")+String(latest_temp)+String(" %"), sensors_color_11);

    //     /// show chart
    //     uint16_t h; 
    //     rept(uint16_t, i, 0, n_lines-1){
            
    //         h = max(0.0f, humid[start_pos + (n_lines - 1) - i]) * 75 / 100.0;
    //         rept(uint16_t, d, 6 + chart_padding, 6 + chart_padding + 2)
    //             canvas.insert_line(
    //                 POINT<>(chart_row+75,   d + i*col_d), 
    //                 POINT<>(chart_row+75-h, d + i*col_d), 
    //                 sensors_color_18
    //             );
            
    //         h = max(0.0f, temp[start_pos + (n_lines - 1) - i]) * 75 / 50.0;
    //         rept(uint16_t, d, 6 + chart_padding + 4, 6 + chart_padding + 6)
    //             canvas.insert_line(
    //                 POINT<>(chart_row+75,   d + i*col_d), 
    //                 POINT<>(chart_row+75-h, d + i*col_d), 
    //                 sensors_color_19
    //             );
    //     }

    //     /// show legend (max value)
    //     canvas.insert_text(POINT<>(chart_row - 2, 3), "100", sensors_color_12);
    //     canvas.insert_text(POINT<>(chart_row - 2, 146), "50", sensors_color_13);

    //     /// process sw_value() for selected box
    //     if(sw_value() == false){
    //         msg2ser("\t", "sw_value():", "is pressed");
    //         unsigned long start_t = millis();
    //         /// delay 100ms for stable
    //         delay(100);
    //         while ( sw_value() == false ){
    //             if(t_since(start_t) > hold_for_select){
    //                 selected = true;
    //                 break;
    //             }
    //         }
    //         msg2ser("\t", "selected: ", selected);
            
    //         if(selected){
    //             /// clear selected flag
    //             selected = false;
    //             /// process corresponding mode :v 
    //             switch (sel){
    //                 case 2:
    //                     msg2ser("\t", "process ```exit``` button", "!");
    //                     /// exit button
    //                     screen_mode = enum_SCREEN_MODE::NORMAL_MODE;
    //                     while(sw_value() == false) controller::iled_blinky(1);
    //                     return;
    //                 case 1:
    //                     msg2ser("\t", "process ```mode``` button", "!");
    //                     /// reserve feature
    //                     screen_mode = enum_SCREEN_MODE::RESERVED_FEATURE_MODE;
    //                     while(sw_value() == false) controller::iled_blinky(1);
    //                     return;
    //             }
    //         }else{
    //             /// increase sel
    //             sel = (sel < 2)?(sel+1):(0);
    //         }
    //     }

    //     /// insert selected box based on ```sel```
    //     switch (sel){
    //         /// case 0: just do nothin' and hide selected box
    //         case 1:
    //             /// show selected box
    //             canvas.insert_rectangle(POINT<>(btn0-2, 13), selectedBox_W, selectedBox_H, sensors_color_21);
    //             break;
    //         case 2:
    //             /// show selected box
    //             canvas.insert_rectangle(POINT<>(btn0-2, 91), selectedBox_W, selectedBox_H, sensors_color_21);
    //             break;
    //     }

    //     /// show changed
    //     canvas.show();
    // }
}
#endif