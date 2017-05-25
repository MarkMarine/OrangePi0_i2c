//
// Created by Mark Fox on 5/23/17.
//

#ifndef MEGAPRAYERI2C_II2C_H
#define MEGAPRAYERI2C_II2C_H

#include <string>
#include <stdlib.h>
#include <sys/time.h>
#include <stdio.h>
#include <iostream>

class II2c
{
 public:
    II2c(unsigned int SCL_, unsigned int SDA_) {};
    virtual bool init(void)  = 0;
    virtual void send(uint8_t address, uint8_t reg, uint8_t *buf, size_t len) = 0;
    virtual uint8_t receive8(uint8_t address, uint8_t reg) = 0;
    virtual uint16_t receive16(uint8_t address, uint8_t reg) = 0;
    virtual ~II2c() {};
};


#endif //MEGAPRAYERI2C_II2C_H
