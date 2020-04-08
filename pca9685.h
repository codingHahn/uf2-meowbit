#include <stdint.h>
#include <libopencm3/stm32/i2c.h>
#include "i2chelper.h"

#define PCA9685_ADDR 0x40
#define PCA9685_MODE1 0x00
#define PCA9685_MODE2 0x01
#define PCA9685_PRESCALE 0xFE

void init_pca9685(i2c_device *dev);
void setfreq_pca9685(i2c_device *dev, unsigned int freq);

void setpwm_pca9685(i2c_device *dev, uint8_t pin, float on_percent,
		    uint8_t delay_percent);

