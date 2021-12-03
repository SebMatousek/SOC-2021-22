#include <Arduino.h>
#include "Robot.h"

Robot *robot;

long long int timer = -1;
const int timeDelay = 50;
const int stopDistance = 140;
const int checkDelay = 50;
const unsigned int motorSpeedCushion = 6;
const unsigned int distanceCushion = 8;
int lastDistance = stopDistance;

int wallDistances[18];
const int WALL_ANGLE_COUNT = 18;

bool firstWall = true;

void checkSide()
{
  robot->set_servoPos(robot->servoMin);

  delay(150);

  int lidarValue = robot->get_lidar();

  if (lidarValue > lastDistance + stopDistance)
  {
    robot->turnWheels(1, 1);
    robot->turn90("right");
    delay(500);
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

    robot->set_servoPos(90);
    delay(150);

    bool distance_reached = false;
    int speed = 15;

    if(bestAngle < 90)
    {
        robot->motorSpeed(1, -speed);
        robot->motorSpeed(0, speed + 5);
    }
    else
    {
        robot->motorSpeed(0, -speed);
        robot->motorSpeed(1, speed + 5);
    }

    while (!distance_reached)
    {
      int lidarValue = robot->get_lidar();

      if(lidarValue < smallestDistance + 10 && lidarValue > 0)
      {
        robot->stopMotors();
        distance_reached = true;
      } 
    }
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
      delay(300);
      firstWall = false;
    }

    timer = -1;
    
    robot->turn90("left");
    delay(300);
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