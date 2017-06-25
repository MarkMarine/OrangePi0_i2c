//
// Created by Mark Fox on 5/21/17.
//

#ifndef ORANGEPI0_I2C_I2C_H
#define ORANGEPI0_I2C_I2C_H

// This has been tuned with a logic analyzer to be
// ~100khz when using the Orange Pi Zero h2+
#define CLK_WIDTH 600

#define INPUT 0
#define OUTPUT 1

#define LOW 0
#define HIGH 1

#include <cstdlib>
#include <stdint.h>
#include <mutex>

using namespace std;

class I2c {
private:
    void delay(void);
    void startBit(void);
    void stopBit(void);
    int tx(uint8_t *buf, size_t len);
    unsigned char rx(unsigned int ack);
    const unsigned int SCL;
    const unsigned int SDA;
    mutex comms_mutex;

public:
    I2c(unsigned int SCL_, unsigned int SDA_);
    bool init(void);
    void send(uint8_t address, uint8_t reg, uint8_t *buf, size_t len);
    uint8_t receive8(uint8_t address, uint8_t reg);
    uint16_t receive16(uint8_t address, uint8_t reg);

    ~I2c();
};


#endif //ORANGEPI0_I2C_I2C_H
