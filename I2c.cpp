//
// Created by Mark Fox on 5/21/17.
//

#include "I2c.h"
#include <cstdio>
#include "gpio_lib.h"

using namespace std;

I2c::I2c(unsigned int SCL_, unsigned int SDA_) :
        SCL(SCL_), SDA(SDA_)
{
}


bool I2c::init(void)
{
    if (sunxi_gpio_init() < 0) {
        printf("ERROR: sunxi_gpio_init() failed");
        return false;
    }

    sunxi_gpio_set_cfgpin(SCL, OUTPUT);
    sunxi_gpio_set_cfgpin(SDA, OUTPUT);
    printf("init finished\n");

    return true;
}

void I2c::delay(void)
{
    for (int delay = CLK_WIDTH; delay; delay--);
}

void I2c::startBit(void)
{
    // To start a transaction, SDA is pulled low while SCL remains high
    // the next step is to pull SCL low.
    sunxi_gpio_output(SCL, HIGH);
    delay();
    sunxi_gpio_set_cfgpin(SDA, OUTPUT);

    sunxi_gpio_output(SDA, HIGH);
    delay();
    sunxi_gpio_output(SDA, LOW);
    delay();
    sunxi_gpio_output(SCL, LOW);
    delay();
}

void I2c::stopBit(void)
{
    // Releasing SDA to float high again would be a stop marker
    // signaling the end of a bus transaction.
    sunxi_gpio_set_cfgpin(SDA, OUTPUT);

    sunxi_gpio_output(SDA, LOW);
    delay();
    sunxi_gpio_output(SCL, HIGH);
    delay();
    sunxi_gpio_output(SDA, HIGH);
    delay();
}

int I2c::tx(uint8_t *buf, size_t len)
{
    sunxi_gpio_set_cfgpin(SDA, OUTPUT);

    int b;
    unsigned char bit;
    for (size_t i = 0; i < len; i++) {
        bit = 0x80;
        for (b = 0; b < 8; b++) {
            sunxi_gpio_output(SDA, (buf[i] & bit));
            delay();

            sunxi_gpio_output(SCL, HIGH);
            delay();

            bit >>= 1;
            sunxi_gpio_output(SCL, LOW);
        }
    }
    delay();
    sunxi_gpio_output(SDA, HIGH);
    sunxi_gpio_output(SCL, HIGH);
    delay();

    // Read SDA for the (N)ACK
    sunxi_gpio_set_cfgpin(SDA, INPUT);
    int ack = sunxi_gpio_input(SDA);
    sunxi_gpio_output(SCL, LOW);
    delay();
    return ack;
}

uint8_t I2c::rx(unsigned int ack)
{
    uint8_t dat = 0;
    // receive the byte.
    for (int i = 0; i < 8; ++i) {
        dat <<= 1;

        // Wait for any clock stretching. If any chip is latching the scl line low,
        // we need to wait for it, our transmission won't be heard if we don't.
        int free;
        do {
            sunxi_gpio_set_cfgpin(SCL, OUTPUT);
            sunxi_gpio_output(SCL, HIGH);
            sunxi_gpio_set_cfgpin(SCL, INPUT);
            free = sunxi_gpio_input(SCL);
        } while (!free);
        delay();

        sunxi_gpio_set_cfgpin(SDA, INPUT);
        int up = sunxi_gpio_input(SDA);
        // Read SDA, if it's HIGH we |= 1, if not it'll be a zero
        if (up) dat |= 1;

        // Pulse the clock back down, done reading this one
        sunxi_gpio_set_cfgpin(SCL, OUTPUT);
        sunxi_gpio_output(SCL, LOW);
        delay();
        // continue reading
    }

    // send the (n)ack
    sunxi_gpio_set_cfgpin(SDA, OUTPUT);
    if (ack) {
        sunxi_gpio_output(SDA, LOW);
    } else {
        sunxi_gpio_output(SDA, HIGH);
    }
    sunxi_gpio_output(SCL, HIGH);
    delay(); // this is the wait for the (n)ack

    sunxi_gpio_output(SCL, LOW);
    delay();
    sunxi_gpio_output(SDA, HIGH);
    return dat;
}


void I2c::send(uint8_t address, uint8_t reg, uint8_t *buf, size_t len)
{
    uint8_t write = address << 1; // Write address
    size_t addLen = 1;

    lock_guard<mutex> guard(comms_mutex);
    startBit();
    int ack = tx(&write, addLen);
    //if (!ack) printf("ACK failed!\n"); // TODO should raise exception here
    ack = tx(&reg, addLen);
    //if (!ack) printf("ACK failed!\n");
    ack = tx(buf, len);
    //if (!ack) printf("ACK failed!\n");
    stopBit();
}

uint8_t I2c::receive8(uint8_t address, uint8_t reg)
{
    uint8_t write = address << 1; // Write address
    uint8_t read = write + (uint8_t )1; // read address, final bit turned on
    size_t len = 1;

    lock_guard<mutex> guard(comms_mutex);
    startBit();
    // Address the board (write)
    tx(&write, len);
    // tell it what address to read from
    tx(&reg, len);
    // repeated start
    startBit();
    // Address the board (read)
    tx(&read, len);
    // Read a single byte and send NACK
    uint8_t val = rx(0);
    // Stop
    stopBit();
    return val;
}

uint16_t I2c::receive16(uint8_t address, uint8_t reg)
{
    uint8_t write = address << 1; // Write address is 7bit add << 1 with 8th bit as 0
    uint8_t read = write + (uint8_t )1; // read address, final bit turned on

    size_t len = 1;

    lock_guard<mutex> guard(comms_mutex);
    startBit();
    tx(&write, len); // send the address
    tx(&reg, len); // send the register we want to read
    startBit(); // restart
    tx(&read, len); // start the read
    uint8_t l = rx(1); // byte 0 + ACK
    uint8_t h = rx(0); // byte 1 + NACK
    stopBit();

    uint16_t val = (uint16_t)l;
    val |= h << 8;

    return val;
}

I2c::~I2c()
{

}
