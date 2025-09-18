#ifndef __HELPER_H__
#define __HELPER_H__

/// HELPERS ///////////////////////////////////////////////////////////////////////////////////////

#ifndef ABS
    #define ABS(x) (((x)>=0)?(x):((-1)*(x)))
#endif 
#ifndef MIN 
    #define MIN(a, b) (((a)<(b))?(a):(b))
#endif 
#ifndef MAX 
    #define MAX(a, b) (((a)>(b))?(a):(b))
#endif 
#ifndef REPT
    #define REPT(type, var, start, end) for(type var = (start); (var) < (end); ++(var))
#endif
#ifndef REVT
    #define REVT(type, var, start, end) for(type var = (start); (var) > (end); --(var))
#endif 
#ifndef TRUE
    #define TRUE 0x1  
#endif 
#ifndef FALSE
    #define FALSE 0x0 
#endif

#ifndef __mask8
    #define __mask8(i)                      (((uint8_t)1) << (i))
#endif
#ifndef __mask32
    #define __mask32(i)                     (((uint32_t)1)<< (i))
#endif
#ifndef __mask64
    #define __mask64(i)                     (((uint64_t)1)<< (i))
#endif
#ifndef __inv_mask8
    #define __inv_mask8(i)                  ((uint8_t)(~((uint8_t)1 << (i))))
#endif
#ifndef __inv_mask32
    #define __inv_mask32(i)                 ((uint32_t)(~((uint32_t)1 << (i))))
#endif
#ifndef __inv_mask64
    #define __inv_mask64(i)                 ((uint64_t)(~((uint64_t)1 << (i))))
#endif

#define __is_zero(x)            ((x) == 0)
#define __is_positive(x)        ((x) >  0)
#define __is_negative(x)        ((x) <  0)
#define __is_not_negative(x)    ((x) >= 0)
#define __is_null(x)            ((x) == NULL)
#define __is_not_null(x)        ((x) != NULL)

/// FLAGS /////////////////////////////////////////////////////////////////////////////////////////

typedef uint64_t flag64_t;

#define __has_flag64(flagNum)	    & 	__mask64(flagNum)
#define __set_flag64(flagNum)	    |= 	__mask64(flagNum)
#define __clr_flag64(flagNum)       &= 	__inv_mask64(flagNum)

/// MUTEX /////////////////////////////////////////////////////////////////////////////////////////


/// LOG ///////////////////////////////////////////////////////////////////////////////////////////
#ifndef __esp_log__
/// Defined in espidf-helper.h
#define __esp_log__

    #include "esp_log.h"
    #include "rom/ets_sys.h"
    #include "esp_timer.h"

    #ifndef __entry
        #define __entry(fmt, ...)   ets_printf("[%lld] [>>>] " fmt "\n", esp_timer_get_time(), ##__VA_ARGS__)
    #endif
    #ifndef __exit
        #define __exit(fmt, ...)    ets_printf("[%lld] [<<<] " fmt "\n", esp_timer_get_time(), ##__VA_ARGS__)
    #endif
    #ifndef __log
        #define __log(fmt, ...)     ets_printf("[%lld] [log] " fmt "\n", esp_timer_get_time(), ##__VA_ARGS__)
    #endif
    #ifndef __tag_log
        #define __tag_log(tag, fmt, ...)     ets_printf("[%lld] [%s] " fmt "\n", esp_timer_get_time(), tag, ##__VA_ARGS__)
    #endif
    #ifndef __err
        #define __err(fmt, ...)     ets_printf("[%lld] [err] " fmt "\n", esp_timer_get_time(), ##__VA_ARGS__)
    #endif

    #define __guess_log(...)    __log(__VA_ARGS__) 
    #define __guess_entry(...)  __entry(__VA_ARGS__)
    #define __guess_exit(...)   __exit(__VA_ARGS__)

#endif

/// DELAY /////////////////////////////////////////////////////////////////////////////////////////

#ifndef __delay__
/// Defined in espidf-helper.h
#define __delay__

    #include "esp_rom_sys.h"

    /// Convert millisecond --> microsecond
    /// NOTE: MAX: 2700 milli-second
    #define __MSEC(i)  ((i) * 1000U)
    /// Convert second --> microsecond
    /// NOTE: MAX: 2SEC
    #define __SEC(i)   ((i) * 1000000U)

    typedef uint32_t millisecTime_t;

    #ifndef microSecDelay
        #define microSecDelay(usec) esp_rom_delay_us(usec);
    #endif 

#endif

/// GPIO HELPERS ///////////////////////////////////////////////////////////////////////////////////

#ifndef __gpioGetSetLevel__
/// Defined in espidf-helper.h
#define __gpioGetSetLevel__
    #include "hal/gpio_ll.h"
    #include "soc/gpio_struct.h"

    /// Set level of output
    static inline void gpioSetLevel(gpio_num_t gpio, bool level) {
        if (gpio < 32) {
            if (level) {
                GPIO.out_w1ts = __mask32(gpio);
            } else {
                GPIO.out_w1tc = __mask32(gpio);
            }
        } else {
            if (level) {
                GPIO.out1_w1ts.data = __mask32(gpio - 32);
            } else {
                GPIO.out1_w1tc.data = __mask32(gpio - 32);
            }
        }
    }

    /// Get level of a GPIO pin
    static inline bool gpioGetLevel(gpio_num_t gpio) {
        if (gpio < 32) {
            return (GPIO.in >> gpio) & 0x1;
        } else {
            return (GPIO.in1.data >> (gpio - 32)) & 0x1;
        }
    }

#endif

#endif 