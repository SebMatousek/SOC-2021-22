#include <Arduino.h>
#include "Robot.h"

Robot *robot;

long long int timer = -1;
const int timeDelay = 50;
const int stopDistance = 140;
const int checkDelay = 50;
const unsigned int motorSpeedCushion = 7;
const unsigned int distanceCushion = 7;
int lastDistance = stopDistance;

int wallDistances[18];
const int WALL_ANGLE_COUNT = 18;

bool firstWall = true;

void turnByAngle(int angle)
{
  robot->delete_enc_value(0);
  robot->delete_enc_value(1);

  if(angle > 0)
  {
    robot->motorSpeed(0, 50);
    robot->motorSpeed(1, -50);
  }
  else
  {
    robot->motorSpeed(0, -50);
    robot->motorSpeed(1, 50);
  }

  bool finished0 = false;
  bool finished1 = false;

  int rotations0 = robot->revolutionClicks * abs(angle / 180.0);
  int rotations1 = robot->revolutionClicks * abs(angle / 180.0);

  //Serial.println(rotations0);
  //Serial.println(rotations1);

  while(!finished0 || !finished1)
  {
    if(abs(robot->get_enc_value(0)) > rotations0)
    {
      finished0 = true;
    }
    if(abs(robot->get_enc_value(1)) > rotations1)
    {
      finished1 = true;
    }
  }

  robot->stopMotors();
}

void checkSide()
{
  robot->set_servoPos(robot->servoMin);

  delay(150);

  int lidarValue = robot->get_lidar();

  if (lidarValue > lastDistance + stopDistance)
  {
    robot->turnWheels(1, 1);
    turnByAngle(97);
    delay(1000);
    robot->motorSpeed(0, 100);
    robot->motorSpeed(1, 100);
  }
  else if (lidarValue > stopDistance + distanceCushion)
  {
    if (robot->get_motor_speed(1) > 50 + motorSpeedCushion)
    {
      robot->motorSpeed(1, robot->get_motor_speed(1) - motorSpeedCushion);
    }

    //Serial.println("to far");
  }
  else if (lidarValue < stopDistance - distanceCushion)
  {
    if (robot->get_motor_speed(0) > 50 + motorSpeedCushion)
    {
      robot->motorSpeed(0, robot->get_motor_speed(0) - motorSpeedCushion);
    }

    //Serial.println("to close");
  }
  else
  {
    //Serial.println("sweetspot");
    robot->motorSpeed(0, 100);
    robot->motorSpeed(1, 100);
  }

  lastDistance = lidarValue;
}

bool checkForward()
{
  robot->set_servoPos(95);

  delay(140);

  int lidarValue = robot->get_lidar();

  if (lidarValue < stopDistance && lidarValue > 0)
  {
    return false;
  }

  return true;
}

void amInWall();

void rideAlongWall()
{
  robot->motorSpeed(0, 100);
  robot->motorSpeed(1, 100);

  while (checkForward())
  {
    delay(checkDelay);
    checkSide();
    delay(checkDelay);
  }

  amInWall();
}

void measureWallAngle()
{
  for (int i = 0; i < WALL_ANGLE_COUNT; i++)
  {
    wallDistances[i] = -1;
  }

  for (int i = 0; i < 180; i += 10)
  {
    robot->set_servoPos(i);
    delay(150);
    wallDistances[i / 10] = robot->get_lidar();
    delay(10);
  }

  int smallestDistance = 8200;
  int bestAngle = 0;

  for (int i = 0; i < WALL_ANGLE_COUNT; i++)
  {
    //Serial.println(wallDistances[i]);
    if (smallestDistance > wallDistances[i] && wallDistances[i] > 0)
    {
      smallestDistance = wallDistances[i];
      bestAngle = i * 10;
    }
  }

  //Serial.println(bestAngle);

  if (bestAngle != 90)
  {
    turnByAngle(90 - bestAngle);
  }
}

void amInWall()
{
  //Serial.println("timer went off");

  int lidarValue = robot->get_lidar();

  if (lidarValue < stopDistance && lidarValue > 0)
  {
    //Serial.println(lidarValue);
    robot->stopMotors();

    if(firstWall)
    { 
      measureWallAngle();
      delay(1000);
      firstWall = false;
    }

    timer = -1;
    
    turnByAngle(-95);
    delay(1000);
    rideAlongWall();
  }
  else
  {
    timer = millis();
  }
}

void getToWall()
{
  robot->motorSpeed(0, 100);
  robot->motorSpeed(1, 100);

  timer = millis();
}

void setup()
{
  robot = new Robot();

  getToWall();
}

void loop()
{
  if (timer != -1 && millis() > timer + timeDelay)
  {
    amInWall();
  }
}