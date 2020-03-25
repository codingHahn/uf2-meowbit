#include "bl.h"
#include "i2chelper.h"
#include <libopencm3/stm32/i2c.h>
#include <stdint.h>

#define PCA9685_ADDR 0x40

#define PCA9685_MODE1 0x00
#define PCA9685_MODE2 0x01
#define PCA9685_PRESCALE 0xFE
#define PCA9685_LED0_ON_L 0x06


void setfreq_pca9685(i2c_device* dev, unsigned int freq) {
  /* 
   * Determining the prescale factor is done based on the formula on page 25
   * or section `7.3.5 PWM Frequency PRE_SCALE` in the datasheet
   */
  uint32_t prescale_val = 25000000;
  prescale_val /= 4096;
  prescale_val /= freq;
  prescale_val -= 1;

  uint8_t mode1;
  read_register8(dev->i2c, dev->i2c_addr, PCA9685_MODE1, &mode1);
  uint8_t reset_mode1 = (mode1 & 0x7F) | 0x10;

  write_register8(dev->i2c, dev->i2c_addr, PCA9685_MODE1, reset_mode1);
  write_register8(dev->i2c, dev->i2c_addr, PCA9685_PRESCALE, (uint8_t)prescale_val);
  write_register8(dev->i2c, dev->i2c_addr, PCA9685_MODE1, mode1);
  delay(100);
  write_register8(dev->i2c, dev->i2c_addr, PCA9685_MODE1, mode1 | 0xa1);
}

void init_pca9685(i2c_device *dev) {
  write_register8(dev->i2c, dev->i2c_addr, PCA9685_MODE1, 0x00);
  setfreq_pca9685(dev, 50);
}

void setdutycycle_pca9685(i2c_device *dev, uint8_t pin, uint16_t on, uint16_t off) {
  if (pin > 15)
    return;
  uint8_t buffer[5];
  buffer[0] = PCA9685_LED0_ON_L + 4 * pin;
  buffer[1] = (uint8_t)on;
  buffer[2] = (uint8_t)(on >> 8);
  buffer[3] = (uint8_t)off;
  buffer[4] = (uint8_t)(off >> 8);

  i2c_transfer7(dev->i2c, dev->i2c_addr, buffer, 5, NULL, 0);
}

void setpwm_pca9685(i2c_device *dev, uint8_t pin, uint8_t on_percent, uint8_t delay_percent) {
  if (on_percent < 1 || on_percent > 100)
    return;

  float on_tmp = 0;
  float on_time = 0;
  float off_tmp = 0;

  if (delay_percent == 0)
    on_tmp = 0;
  else
    on_tmp = 4096 * (((float)delay_percent) / 100) - 1;
  on_time = 4096 * (((float)on_percent) / 100) - 1;
  off_tmp = on_tmp + on_time - 1;

  uint16_t on = (uint16_t)on_tmp;
  uint16_t off = (uint16_t)off_tmp;

  setdutycycle_pca9685(dev, pin, on, off);
}
