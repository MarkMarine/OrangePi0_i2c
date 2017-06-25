//
// Created by Mark Fox on 5/24/17.
//

#ifndef ORANGEPI0_I2C_I2CFACTORY_H
#define ORANGEPI0_I2C_I2CFACTORY_H

#include "I2c.h"
#include <memory>

using namespace std;

class I2cFactory {

public:
    I2cFactory();
    shared_ptr<I2c> create_I2c(unsigned int scl, unsigned int sda);
};


#endif //ORANGEPI0_I2C_I2CFACTORY_H
