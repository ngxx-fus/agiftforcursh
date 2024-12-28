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
        ResetFunc ();
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

#endif