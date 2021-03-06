//
// Created by Mark Fox on 5/24/17.
//

#include <err.h>
#include "I2cFactory.h"
#include <memory>

using namespace std;

I2cFactory::I2cFactory()
{

}

shared_ptr<I2c> I2cFactory::create_I2c(unsigned int scl, unsigned int sda) {
    auto i2c = make_shared<I2c>(scl, sda);
    i2c.reset(new I2c(scl, sda));

    if (!i2c->init()) {
        err(1, "i2c->init() failed");
        return NULL;
    }

    return i2c;
}
