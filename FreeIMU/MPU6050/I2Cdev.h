//ported from arduino library: https://github.com/jrowberg/i2cdevlib/tree/master/Arduino/MPU6050
//written by szymon gaertig (email: szymon@gaertig.com.pl), small correction by Aloïs Wolff
//
//Changelog: 
//2013-01-08 - first beta release
//2013-02-18 - fixed a malloc with no free in ::readBytes

#ifndef I2Cdev_h
#define I2Cdev_h

#ifndef I2C_SDA
    #define I2C_SDA p28
    #define I2C_SCL p27
#endif

#include "mbed.h"
#include "MODI2C.h"

class I2Cdev {
    private:
        
        Serial debugSerial;
    public:
        I2Cdev();
        I2Cdev(PinName i2cSda, PinName i2cScl);        
        I2Cdev(MODI2C i2c_);
        
        int8_t readBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t *data, uint16_t timeout=I2Cdev::readTimeout());
        int8_t readBitW(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint16_t *data, uint16_t timeout=I2Cdev::readTimeout());
        int8_t readBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t *data, uint16_t timeout=I2Cdev::readTimeout());
        int8_t readBitsW(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint16_t *data, uint16_t timeout=I2Cdev::readTimeout());
        int8_t readByte(uint8_t devAddr, uint8_t regAddr, uint8_t *data, uint16_t timeout=I2Cdev::readTimeout());
        int8_t readWord(uint8_t devAddr, uint8_t regAddr, uint16_t *data, uint16_t timeout=I2Cdev::readTimeout());
        int8_t readBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data, uint16_t timeout=I2Cdev::readTimeout());
        int8_t readWords(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint16_t *data, uint16_t timeout=I2Cdev::readTimeout());

        bool writeBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t data);
        bool writeBitW(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint16_t data);
        bool writeBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t data);
        bool writeBitsW(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint16_t data);
        bool writeByte(uint8_t devAddr, uint8_t regAddr, uint8_t data);
        bool writeWord(uint8_t devAddr, uint8_t regAddr, uint16_t data);
        bool writeBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *data);
        bool writeWords(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint16_t *data);
        
        void readBytes_nb(uint8_t devAddr, char *command, uint8_t length, uint8_t *data, uint32_t (*function)(uint32_t), void* param);

        static uint16_t readTimeout(void);

        MODI2C i2c;
        
        char* allocbuffer();
        void freebuffer();
        
        MemoryPool<char[14], 16> pool;
        Queue<char, 16> queue;
};

#endif