#ifndef __SPI_H__
#define __SPI_H__

/// HEADERS ///////////////////////////////////////////////////////////////////////////////////////

#include "stdint.h"
#include "stdarg.h"
#include "stdio.h"
#include "stdbool.h"
#include "driver/gpio.h"

#include "../../include/espidf-helper.h"

/// DATATYPES /////////////////////////////////////////////////////////////////////////////////////

typedef int8_t              pin_t;      /// Signed byte, -1 meaning unused
typedef uint8_t             byte_t;     /// Byte type (aka unsigned char)
typedef uint16_t            word_t;     /// Word type (aka 16-bit unsigned int)
typedef uint32_t            dword_t;    /// Double-word (aka 32-bit unsigned int)
typedef uint64_t            qword_t;    /// Quadruple word (aka 64-bit unsigned int)
typedef void (*isrFunction)(void);

#define spiBool(x)       ((x)?(1):(0))

// #define MAX_BUFF_SIZE       16

enum SPI_PIN_STATE{
    SPI_UNUSED = -1,
};

enum SPI_MASK_BIT{
    CPOL = 1,               /// Idle state level ([0]:LOW, [1]:HIGH)
    CPHA = 2,               /// [0]: Sample at 1st / [1]: Sample at 2nd edge
                            /// NOTE: if CPHA=[0], Data must be stable before the first clock edge.
    HOST_TYPE,              /// Host type: [0]:Master / [1]:Slave
    BIT_ORDER               /// The order of send/receive. [0]:MSB->LSB, [1]:LSB->MSB
};

typedef struct spiDevice_t{
    pin_t   CLK;            /// Store CLK pin
    pin_t   MOSI;           /// Store MOSI (master out, slave in) pin 
    pin_t   MISO;           /// Store MISO (master in, slave out) pin
    pin_t   CS;             /// Chip select
    byte_t  conf;           /// SPI's config
    byte_t  status;         /// SPI's status
    qword_t clkFreq;        /// CLK frequency
    qword_t clkPeriod;      /// CLK period in microsecond, it can be auto compute on  
    byte_t  clkDutyCycle;   /// CLK duty cycle, Tactive / (Tactive+Tidel), [0...100]
    qword_t spiTransactionLevel;    /// 
    #ifdef MAX_BUFF_SIZE
        byte_t* sendBuff;       /// SPI send buffer
        dword_t sendBuffInd;    /// SPI send buffer index
        dword_t sendBuffSize;   /// SPI send buffer size
        byte_t* receiveBuff;    /// SPI receive buffer
        dword_t receiveBuffInd; /// SPI receive buffer index
        dword_t receiveBuffSize;/// SPI receive buffer size
    #endif
}spiDevice_t;

#define __clk(x)                ((x)->CLK)
#define __mosi(x)               ((x)->MOSI)
#define __cs(x)                 ((x)->CS)
#define __clkPeriod(x)          ((x)->clkPeriod)
#define __clkDutyCycle(x)       ((x)->clkDutyCycle)


/// SPI CORE  /////////////////////////////////////////////////////////////////////////////////////

// extern byte_t spiTransactionLevel;

static inline void spiStartTransaction(spiDevice_t * spiDev){
    /// Start a spi transaction 
    if((spiDev->spiTransactionLevel) == 0){
        /// If the transaction hasn't been started
        gpioSetLevel(spiDev->CS, 0);
        ++(spiDev->spiTransactionLevel);
    }else{
        /// If the transaction has been started
    }
}

static inline void spiEndTransaction(spiDevice_t * spiDev){
    /// Start a spi transaction 
    if((spiDev->spiTransactionLevel) == 0){
        /// If the transaction has been end 
        gpioSetLevel(spiDev->CS, 1);
    }else{
        --(spiDev->spiTransactionLevel);
        /// If the transaction hasn't been end
    }
}

#define spiSetCLK(dev, level) gpioSetLevel(__clk(dev), level)
#define spiSetMOSI(dev, level) gpioSetLevel(__mosi(dev), level)
#define spiSetCS(dev, level) gpioSetLevel(__cs(dev), level)

/// SPI DRIVER ////////////////////////////////////////////////////////////////////////////////////

/// @brief Create new SPI Device then return the address of SPIDEV
/// @param spiDev A pointer point to where stores SPI Device
/// @param CLK CLK pin
/// @param MOSI MOSI pin
/// @param MISO Currently unused, pls set [0]
/// @param CS CS pin
/// @param clkFreq The frequency, The valid range is [1Hz, 1000000Hz]
/// @param clkDutyCycle The duty cycle of CLK, set [0] to set default: 50.
spiDevice_t *createNewTFTSPIDev(pin_t CLK, pin_t MOSI, pin_t MISO, pin_t CS, qword_t clkFreq, byte_t  clkDutyCycle);

/// @brief Just sending a byte (not control CS pin)
/// @param dev Pointer to spiDevice_t obj
/// @param data8 Data uses to be sent
/// @return 
void spiSendByte(spiDevice_t * dev, byte_t data8);

/// @brief Just sending a word (2xbyte) (not control CS pin)
/// @param dev Pointer to spiDevice_t obj
/// @param data16 Data uses to be sent
/// @return 
void spiSendWord(spiDevice_t * dev, word_t data16);

/// TFT DRIVER ////////////////////////////////////////////////////////////////////////////////////



#endif