#include "servo.h"
#include "i2chelper.h"
typedef struct {
  Servo base;
  Servo tip;
  uint8_t base_deg;
  uint8_t tip_deg;
  uint8_t offset;
} leg_t;

extern leg_t leg1;
extern leg_t leg2;
extern leg_t leg3;
extern leg_t leg4;

void move_leg(i2c_device *dev, leg_t *leg);
