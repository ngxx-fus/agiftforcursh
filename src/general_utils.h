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
#endif