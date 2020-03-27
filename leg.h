#include "servo.h"
#include "i2chelper.h"
typedef struct {
  Servo base;
  Servo tip;
  int8_t base_deg;
  int8_t tip_deg;
  int8_t multiplier;
  uint8_t offset;
} leg_t;

extern leg_t leg1;
extern leg_t leg2;
extern leg_t leg3;
extern leg_t leg4;

void move_leg(i2c_device *dev, leg_t *leg);
void move_leg_to_deg(i2c_device *dev, leg_t *leg, int8_t deg);
void move_leg_on_ground(i2c_device *dev, leg_t *leg, int8_t deg);
void lift_leg(i2c_device *dev, leg_t *leg);
void stand_up(i2c_device *dev, leg_t *leg1, leg_t *leg2, leg_t *leg3,
              leg_t *leg4);
