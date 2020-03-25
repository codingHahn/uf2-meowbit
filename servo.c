#include "i2chelper.h"
#include "pca9685.h"
#include "servo.h"

void servo_set_deg(i2c_device *dev, Servo s, int16_t deg) {
if(deg > 90 || deg < -90)
  return;
deg += 90;
uint8_t max_percent = 10;
uint8_t min_percent = 5;
uint8_t max_deg = 180;
uint8_t min_deg = 0;

float resulting_on_precent = ((float)(deg - min_deg)) / (max_deg - min_deg) * 
	(max_percent - min_percent) + min_percent;

setpwm_pca9685(dev, s + 7, (uint8_t)resulting_on_precent, 10);
}
