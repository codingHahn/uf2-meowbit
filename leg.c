#include "leg.h"
#include "bl.h"

/*
 * legs of robot. Global variables
 */
leg_t leg1 = {.base = S2, .tip = S1, .base_deg = 0, .tip_deg = 0, .offset = 0, .multiplier = -1};
leg_t leg2 = {.base = S4, .tip = S3, .base_deg = 0, .tip_deg = 0, .offset = 20, .multiplier = 1};
leg_t leg3 = {.base = S6, .tip = S5, .base_deg = 0, .tip_deg = 0, .offset = 40, .multiplier = -1};
leg_t leg4 = {.base = S8, .tip = S7, .base_deg = 0, .tip_deg = 0, .offset = 60, .multiplier = 1};

/*
 * Move both base and tip to position specified in *leg
 */
void move_leg(i2c_device *dev, leg_t *leg) {
  servo_set_deg(dev, leg->base, leg->base_deg, leg->offset);
  servo_set_deg(dev, leg->tip, leg->tip_deg, leg->offset + 10);
}

/*
 * Lift *leg up 90 degrees
 */
void lift_leg(i2c_device *dev, leg_t *leg) {
  servo_set_deg(dev, leg->tip, 80*leg->multiplier, leg->offset + 10);
  leg->tip_deg = 80*leg->multiplier;
}

/*
 * Lower *leg to -30 degrees
 */
void lower_leg(i2c_device *dev, leg_t *leg) {
  servo_set_deg(dev, leg->tip, (-30)*leg->multiplier, leg->offset + 10);
  leg->tip_deg = (-30)*leg->multiplier;
}

/*
 * Move *leg base to deg while lifting the tip at the beginning of movement
 * and lower the tip at the end to its original value
 */
void move_leg_to_deg(i2c_device *dev, leg_t *leg, int8_t deg) {
  servo_set_deg(dev, leg->tip, leg->tip_deg / 6 * (-1), leg->offset + 10);
  delay(500);
  servo_set_deg(dev, leg->base, deg, leg->offset);
  delay(500);
  servo_set_deg(dev, leg->tip, leg->tip_deg, leg->offset + 10);
  leg->base_deg = deg;
}

/*
 * Move *leg base to deg without lifting its tip
 */
void move_leg_on_ground(i2c_device *dev, leg_t *leg, int8_t deg) {
  servo_set_deg(dev, leg->base, deg, leg->offset); 
  leg->base_deg = deg;
}
