//
// Created by Mark Fox on 5/24/17.
//

#ifndef MEGAPRAYERI2C_I2CFACTORY_H
#define MEGAPRAYERI2C_I2CFACTORY_H


#include "I2c.h"

class I2cFactory {

public:
    I2cFactory();
    std::shared_ptr<I2c> create_I2c(unsigned int scl, unsigned int sda);
};


#endif //MEGAPRAYERI2C_I2CFACTORY_H
