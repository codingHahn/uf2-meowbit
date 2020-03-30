#include "bl.h"
#include "robot.h"
#include <math.h>

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

void swing(robot_t *robot, uint64_t repetitions) {
  double phaseshift = 90 * (M_PI / 180);
  uint64_t counter = 0;
  
  while(counter < repetitions) {
    //TODO: Count actual cycles and not just this while loop
    //	    Maybe through interrups?
    robot->leg1->tip_deg = sin(counter * (M_PI / 180))* 80* robot->leg1->multiplier;
    robot->leg4->tip_deg = sin(counter * (M_PI / 180))* 80* robot->leg2->multiplier;
    robot->leg2->tip_deg = sin(counter * (M_PI / 180)+phaseshift)* 80* robot->leg4->multiplier;
    robot->leg3->tip_deg = sin(counter * (M_PI / 180)+phaseshift)* 80* robot->leg3->multiplier;
    counter++;
    move_leg(robot->dev, robot->leg1);
    move_leg(robot->dev, robot->leg2);
    move_leg(robot->dev, robot->leg3);
    move_leg(robot->dev, robot->leg4);
    delay(5);
  }
}
