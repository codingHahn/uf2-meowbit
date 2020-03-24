#include "i2chelper.h"
#include "pca9685.h"

typedef enum{
  S1 = 0x01,
  S2 = 0x02,
  S3 = 0x03,
  S4 = 0x04,
  S5 = 0x05,
  S6 = 0x06,
  S7 = 0x07,
  S8 = 0x08
} Servo;

void servo_set_deg(i2c_device *dev, Servo s, int16_t deg);
