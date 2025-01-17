#ifndef GENERAL_UTILS_H
#define GENERAL_UTILS_H
#include <Arduino.h>
#include <serial_utils.h>

#ifndef rep
#define rep(i,a,b) for(int i = (a); i <= (b); ++i)
#endif
#ifndef rev
#define rev(i,a,b) for(int i = (b); i >= (a); --i)
#endif
#ifndef revt
#define rept(type, i,a,b) for(type i = (a); i <= (b); ++i)
#endif
#ifndef revt
#define revt(type, i,b, a) for(type i = (b); i >= (a); --i)
#endif

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

/// @brief A class that manages time-based delays and executes an action after a specified interval.
/// This class keeps track of the time elapsed since the `initial time` (stored in `last_t`), 
/// and compares it with a user-defined `interval`. If the time difference exceeds or equals 
/// the `interval`, the `action` function is executed.
class DELAY_CTL {
    uint32_t last_t, interval;   ///< The last time the action was executed or the initial time.

public:
    /// @brief Default constructor that initializes with the current time and sets the interval to 0.
    /// This constructor sets the `last_t` to the current time (via `millis()`) and sets the `interval` to 0.
    DELAY_CTL(){
        last_t = millis();  ///< Sets `last_t` to the current time.
        interval = 0;        ///< Sets `interval` to 0 by default.
    }

    /// @brief Constructor with a custom interval, initializes action to `nullptr`.
    /// This constructor sets the `last_t` to the current time (via `millis()`) and assigns a user-defined `interval`.
    /// @param interval   The interval time (in milliseconds) to compare with the current time.
    DELAY_CTL(uint32_t interval){
        this->last_t = millis();  ///< Sets the initial time to the current time.
        this->interval = interval; ///< Sets the interval to the provided value.
    }

    /// @brief Sets a new interval time.
    /// This function updates the `interval` to a new value.
    /// @param t   The new interval time (in milliseconds) to set.
    void set_interval(uint32_t t){
        interval = t; ///< Updates `interval` to the new value.
    }

    /// @brief Sets a new initial time for `last_t`.
    /// This method updates the `last_t` variable to a specified time, representing the last action execution time.
    /// @param t   The new initial time (in milliseconds) to set.
    void set_initial_time(uint32_t t){
        last_t = t; ///< Updates `last_t` to the specified time.
    }

    /// @brief Updates the initial time to the current time.
    /// This method resets `last_t` to the current time (via `millis()`), effectively marking the moment of the last action.
    void update_initial_time(){
        last_t = millis(); ///< Resets `last_t` to the current time.
    }

    /// @brief Checks if the specified interval has passed and optionally updates the initial time.
    /// This method compares the elapsed time since the last action and checks if it has exceeded the interval.
    /// If so, it resets `last_t` to the current time.
    /// @param update   A boolean flag to indicate if `last_t` should be updated after the action is run (default is `true`).
    /// @return `true` if the interval has passed, `false` otherwise.
    bool time_to_run(bool update = true){
        if(t_since(last_t) >= interval) {  ///< Checks if the time since the last action exceeds the interval.
            if(update) last_t = millis(); ///< Optionally updates `last_t` to the current time after the check.
            return true; ///< Indicates the action can be run as the interval has passed.
        }
        return false; ///< If the interval hasn't passed, returns `false`.
    }

    /// @brief Checks if the specified interval has passed, runs the action, and optionally resets `last_t`.
    /// This method checks if the interval has passed, then runs the specified action and optionally updates `last_t`.
    /// If no action is provided, it defaults to an empty lambda function (`[]() -> void {}`).
    /// @param action   The function to execute if the interval has passed (defaults to an empty lambda).
    /// @param update   A boolean flag to decide whether to update `last_t` after running the action (default is `true`).
    /// @return `true` if the action was executed and the initial time was updated, `false` otherwise.
    template<class... Targs>
    bool run(std::function<void(Targs...)> action = []() -> void {}, bool update = true){
        if(t_since(last_t) >= interval) {  ///< Checks if the time since the last action exceeds the interval.
            if(action)  ///< If an action is provided, execute it.
                action(); ///< Runs the action with the provided arguments.
            if(update) last_t = millis(); ///< Optionally updates `last_t` to the current time after running the action.
            return true; ///< Indicates the action was performed and the time updated.
        }
        return false; ///< If the interval hasn't passed yet, returns `false`.
    }

    /// @brief Retrieves the current interval time.
    /// This method returns the current interval time in milliseconds.
    /// @return The current interval time in milliseconds.
    uint32_t get_interval() { return this->interval; }

    /// @brief Retrieves the last time the action was executed (the initial time).
    /// This method returns the last time the action was executed, stored in `last_t`.
    /// @return The last time the action was executed (in milliseconds).
    uint32_t get_last_t() { return this->last_t; }
};


String filename_get_extension(String filename){
    String res = "";
    uint8_t dot = 0;
    if(filename.isEmpty()) return res;
    revt(uint8_t, i, filename.length()-1, 0)
        if(filename.charAt(i) != '.'){
            res.concat(filename.charAt(i));
        }else{
            break;
        }
    return res;
}


#endif