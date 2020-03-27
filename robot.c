#include "robot.h"

void stand_up(robot_t *robot, uint8_t deg) { 
  servo_set_deg(robot->dev, robot->leg1->tip, deg*robot->leg1->multiplier, robot->leg1->offset);
  robot->leg1->tip_deg = deg*robot->leg1->multiplier;
  servo_set_deg(robot->dev, robot->leg2->tip, deg*robot->leg2->multiplier, robot->leg2->offset);
  robot->leg2->tip_deg = deg*robot->leg2->multiplier;
  servo_set_deg(robot->dev, robot->leg3->tip, deg*robot->leg3->multiplier, robot->leg3->offset);
  robot->leg3->tip_deg = deg*robot->leg3->multiplier;
  servo_set_deg(robot->dev, robot->leg4->tip, deg*robot->leg4->multiplier, robot->leg4->offset);
  robot->leg4->tip_deg = deg*robot->leg4->multiplier;
}

