#include "leg.h"
#include "bl.h"

leg_t leg1 = {.base = S2, .tip = S1, .base_deg = 0, .tip_deg = 0, .offset = 0, .multiplier = -1};
leg_t leg2 = {.base = S4, .tip = S3, .base_deg = 0, .tip_deg = 0, .offset = 20, .multiplier = 1};
leg_t leg3 = {.base = S6, .tip = S5, .base_deg = 0, .tip_deg = 0, .offset = 40, .multiplier = -1};
leg_t leg4 = {.base = S8, .tip = S7, .base_deg = 0, .tip_deg = 0, .offset = 60, .multiplier = 1};

void move_leg(i2c_device *dev, leg_t *leg) {
  servo_set_deg(dev, leg->base, leg->base_deg, leg->offset);
  servo_set_deg(dev, leg->tip, leg->tip_deg, leg->offset + 10);
}
void lift_leg(i2c_device *dev, leg_t *leg) {
  servo_set_deg(dev, leg->tip, 60*leg->multiplier, leg->offset + 10);
  leg->tip_deg = 60*leg->multiplier;
}
void move_leg_to_deg(i2c_device *dev, leg_t *leg, int8_t deg) {
  servo_set_deg(dev, leg->tip, leg->tip_deg / 6 * (-1), leg->offset + 10);
  delay(500);
  servo_set_deg(dev, leg->base, deg, leg->offset);
  delay(500);
  servo_set_deg(dev, leg->tip, leg->tip_deg, leg->offset + 10);
  leg->base_deg = deg;
}

void move_leg_on_ground(i2c_device *dev, leg_t *leg, int8_t deg) {
  servo_set_deg(dev, leg->base, deg, leg->offset); 
  leg->base_deg = deg;
}

void stand_up(i2c_device *dev, leg_t *leg1, leg_t *leg2, leg_t *leg3,
              leg_t *leg4) {
  
  servo_set_deg(dev, leg1->tip, 60*leg1->multiplier, leg1->offset);
  leg1->tip_deg = 60*leg1->multiplier;
  servo_set_deg(dev, leg2->tip, 60*leg2->multiplier, leg2->offset);
  leg2->tip_deg = 60*leg2->multiplier;
  servo_set_deg(dev, leg3->tip, 60*leg3->multiplier, leg3->offset);
  leg3->tip_deg = 60*leg3->multiplier;
  servo_set_deg(dev, leg4->tip, 60*leg4->multiplier, leg4->offset);
  leg4->tip_deg = 60*leg4->multiplier;
}
