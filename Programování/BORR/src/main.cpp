#include <Arduino.h>
#include "Robot.h"

Robot *robot;

const int revolutionClicks = 4172;

/*
unsigned long long timer = 0;
const unsigned int timeDelay = 100;
const unsigned int padding = 1;

void encCheck()
{
  if(robot->get_relative_motor_speed(0) != 0 && robot->get_relative_motor_speed(1) != 0)
  {
    int mr0 = robot->get_motor_speed(0);
    int mr1 = robot->get_motor_speed(1);

    int m0 = robot->get_relative_motor_speed(0);
    int m1 = robot->get_relative_motor_speed(1);
    int e0 = robot->get_enc_value(0);
    int e1 = robot->get_enc_value(1);

    float motorComp = float(m0) / m1;
    float encComp = float(e0) / e1;

    if (motorComp > encComp)
    {
      Serial.println("left is slower by.. " + String(motorComp) + "  while enc... " + String(encComp));
      if (mr0 <= 100 - padding)
      {
        robot->motorSpeed(0, mr0 + padding, false);
      }
      else if (mr1 >= 0 + padding)
      {
        robot->motorSpeed(1, mr1 - padding, false);
      }
    }
    else if(motorComp < encComp)
    {
      Serial.println("right is slower by.. " + String(motorComp) + "while enc... " + String(encComp));
      if (mr1 <= 100 - padding)
      {
        robot->motorSpeed(1, mr1 + padding, false);
      }
      else if (mr0 >= 0 + padding)
      {
        robot->motorSpeed(0, mr0 - padding, false);
      }
    }

      robot->delete_enc_value(0);
      robot->delete_enc_value(1);

      Serial.println("left... " + String(robot->get_motor_speed(0)));
      Serial.println("right... " + String(robot->get_motor_speed(1)));
  }

  Serial.println("left enc..." + String(robot->get_enc_value(0)));
  Serial.println("right enc..." + String(robot->get_enc_value(1)));
  robot->delete_enc_value(0);
  robot->delete_enc_value(1);
}*/

void turn(int angle)
{
  robot->delete_enc_value(0);
  robot->delete_enc_value(1);

  bool leftTurned = false;
  bool rightTurned = false;

  robot->motorSpeed(0, 100);
  robot->motorSpeed(1, -100);
  
  while(!leftTurned || !rightTurned)
  { 

    Serial.println("left... " + String(robot->get_enc_value(0)));
    Serial.println("right... " + String(robot->get_enc_value(1)));

    if(abs(robot->get_enc_value(0)) > abs(revolutionClicks*0.55))
    {
      robot->motorSpeed(0, 0);
      leftTurned = true;
    }
    if(abs(robot->get_enc_value(1)) > abs(revolutionClicks*0.55))
    {
      robot->motorSpeed(1, 0);
      rightTurned = true;
    }
  }
}

void setup() {
  robot = new Robot();

  robot->motorSpeed(0, 100);
  robot->motorSpeed(1, 100);

  turn(1);
}

void loop() {

}