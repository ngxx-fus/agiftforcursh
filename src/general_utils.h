#ifndef TIME_UTILS_H
#define TIME_UTILS_H
#include <Arduino.h>
#include <serial_utils.h>


/// >>>>>>>>>>>>>>>>>>>>> TIME UTILS >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
#define delta64_time(t_final, t_initial) ((t_final>t_initial)?(t_final-t_initial):(t_initial - t_final - 4294967296))
#define delta32_time(t_final, t_initial) ((t_final>t_initial)?(t_final-t_initial):(t_initial - t_final - 65536      ))
#define delta16_time(t_final, t_initial) ((t_final>t_initial)?(t_final-t_initial):(t_initial - t_final - 256        ))
#define delta8_time(t_final, t_initial)  ((t_final>t_initial)?(t_final-t_initial):(t_initial - t_final - 16         ))

/// @brief return how long since t_initial to now
/// @tparam Ttime ```unsigned``` type
/// @param t_initial the time to calc from 
/// @return ```Ttime``` how long from t_initial to now
/// @note support 64-bit, 32-bit, 16-bit, 8-bit
template<class Ttime>
Ttime t_since(Ttime t_initial){
    if( is_unsigned<Ttime>() == false ){
        msg2ser("\t", "Ttime: signed type");
        delay(1000);
        return -1;
    }
    Ttime t_now = Ttime(millis());
    switch (sizeof(Ttime)){
        case 8:
            return ((t_now>t_initial)?(t_now-t_initial):(t_initial - t_now - 4294967296U ));
        case 4:
            return ((t_now>t_initial)?(t_now-t_initial):(t_initial - t_now - 65536U      ));
        case 2:
            return ((t_now>t_initial)?(t_now-t_initial):(t_initial - t_now - 256U        ));
        case 1:
            return ((t_now>t_initial)?(t_now-t_initial):(t_initial - t_now - 16U         ));
    }
    /// unsupport bit of type :v
    return -1;
}

/// @brief base case for string concatenate
/// @tparam Tstring any type that will be casted to ```String```
/// @param str default parametter
/// @return the string after concatenated
template<class Tstring>
String concatenate(Tstring str){
    return String(str);
}
/// @brief string concatenate
/// @tparam Tstring any type that will be casted to ```String``` 
/// @tparam ...Tstrings extended list parameters, any type that will be casted to ```String``` 
/// @param str default parametter
/// @param ...strs extended list parameters
/// @return the string after concatenated
template<class Tstring, class... Tstrings>
String concatenate(Tstring str, Tstrings ...strs){
    String res = String(str);
    res.concat(concatenate(strs...));
    return res;
}

/// @brief Convert from RGB to 565Color 
/// @return 16bit converted color
/// @note (5 bits at the beginning and the end for R and B, with 6 bits in the middle for G)
template<class TcolorR, class TcolorG, class TcolorB>
uint16_t rgb_to_565(TcolorR r, TcolorG g, TcolorB b) {
    return ((uint16_t(r) & 0xF8) << 8) | ((uint16_t(g) & 0xFC) << 3) | (uint16_t(b) >> 3);
}

/// @brief Converts an integer to a hexadecimal string representation.
/// @param number The integer to be converted to a hexadecimal string.
/// @return A String object containing the hexadecimal representation of the input integer.
/// @note The output string will be in uppercase by default due to the `toUpperCase()` method. 
/// If lowercase is preferred, remove the `toUpperCase()` call.
/// - This function uses Arduino's `String` class with the `HEX` argument to handle the conversion.
/// - For negative integers, the result will be the two's complement hexadecimal representation.
String intToHexString(int number) {
    String hexString = String(number, HEX);
    hexString.toUpperCase();
    return hexString;
}

/// @brief Reverses the bits of a given number.
/// @param num The number whose bits are to be reversed.
/// @return The number with its bits reversed.
template<class T>
unsigned int reverse_bit_order(T num) {
    T reversed = 0;  ///< Variable to store the reversed bits.
    
    int numBits = sizeof(num) * 8;  ///< Calculate the number of bits in the input number.
    
    // Loop through each bit of the input number.
    for (int i = 0; i < numBits; i++) {
        reversed <<= 1;  ///< Shift the reversed number to the left by one bit to make space for the next bit.
        
        reversed |= (num & 1);  ///< Extract the least significant bit (LSB) from num and add it to reversed.
        
        num >>= 1;  ///< Shift num to the right by one bit to process the next bit.
    }
    
    return reversed;  ///< Return the final reversed number.
}

/// @brief  A class to manage a time delay and execute an action after a specified interval.
/// The class tracks the time elapsed since the `initial time` and compares it with the `interval`.
/// If the time difference exceeds or equals the `interval`, the `action` is executed.
class delay_t{
    uint32_t last_t, interval;   ///< The last time the action was executed or the initial time.
    function<void(void)> action; ///< The action to be executed after the interval.

public:
    /// @brief  Default constructor, initializes with current time and interval as 0.
    delay_t(){
        last_t = millis();  ///< Initializes `last_t` with current time.
        interval = 0;        ///< Sets `interval` to 0 by default.
    }

    /// @brief  Constructor with custom interval and action.
    /// @param interval   The interval time (in milliseconds) to compare with the current time.
    /// @param action     The action to be executed when the interval has elapsed.
    delay_t(uint32_t interval, function<void(void)> action){
        this->last_t = millis();  ///< Sets the initial time to the current time.
        this->interval = interval; ///< Sets the interval to the specified value.
        this->action = action;     ///< Sets the action to be executed.
    }

    /// @brief  Sets a new interval time.
    /// @param t   The new interval time (in milliseconds) to set.
    void set_interval(uint32_t t){
        interval = t; ///< Sets the interval to the specified value.
    }

    /// @brief  Sets a new initial time.
    /// @param t   The new initial time (in milliseconds).
    void set_initial_time(uint32_t t){
        last_t = t; ///< Updates the `last_t` to the specified time.
    }

    /// @brief  Updates the initial time to the current time.
    void update_initial_time(){
        last_t = millis(); ///< Resets the `last_t` to the current time.
    }

    /// @brief  Checks if the specified interval has passed and runs the action if needed.
    /// @return `true` if the action was executed, `false` otherwise.
    bool run(){
        if(t_since(last_t) >= interval) {  ///< Checks if the time since the last action exceeds the interval.
            if(action)  ///< If an action is defined.
                action(); ///< Executes the action.
            return true; ///< Indicates that the action was performed.
        }
        return false; ///< If the interval hasn't passed yet, return `false`.
    }

    /// @brief  Updates the initial time and runs the action if the interval has passed.
    /// @return `true` if the action was executed and the initial time was updated, `false` otherwise.
    bool update_and_run(){
        if(t_since(last_t) >= interval) {  ///< Checks if the time since the last action exceeds the interval.
            if(action)  ///< If an action is defined.
                action(); ///< Executes the action.
            last_t = millis(); ///< Resets `last_t` to the current time after running the action.
            return true; ///< Indicates that the action was performed and the initial time updated.
        }
        return false; ///< If the interval hasn't passed yet, return `false`.
    }
};


#endif