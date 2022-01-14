#include <Arduino.h>
#include "Robot.h"

Robot *robot;

long long int dataTimer = 0;
const int dataTimerDelay = 1000;

const int stopDistance = 95;
const int checkDelay = 50;
const unsigned int motorSpeedCushion = 8;
const unsigned int distanceCushion = 6;
int lastDistance = stopDistance;

int wallDistances[18];
const int WALL_ANGLE_COUNT = 18;

bool firstWall = true;

bool mapping = false;

int robotSpeed = 80;

void central(String data)
{
  if (data.equals("-"))
  {
    return;
  }
  else if (data.equals("go"))
  {
    mapping = true;
  }
  else if (data.equals("stop"))
  {
    mapping = false;
  }
}

void readIncoming()
{
  String data = robot->readData();
  central(data);
}

void sendSensorData()
{
  if (millis() > dataTimer + dataTimerDelay)
  {
    dataTimer = millis();
    robot->sendSensorData();
  }
}

void communicateWithPhone()
{
  sendSensorData();
  readIncoming();
}

void checkSide()
{
  robot->set_servoPos(robot->servoMin);

  delay(150);

  int lidarValue = robot->get_lidar();

  if (lidarValue > lastDistance + stopDistance)
  {
    robot->turnWheels(1.25, 1.25);

    robot->sendMapData();

    robot->turnByAngle(90);
    robot->countTurnAngle(90);

    robot->delete_enc_value(0);
    robot->delete_enc_value(1);

    delay(1000);
    robot->motorSpeed(0, robotSpeed);
    robot->motorSpeed(1, robotSpeed);
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
    robot->motorSpeed(0, robotSpeed);
    robot->motorSpeed(1, robotSpeed);
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
  robot->delete_enc_value(0);
  robot->delete_enc_value(1);

  robot->bothMotorSpeed(robotSpeed);

  while (checkForward())
  {
    delay(checkDelay);
    checkSide();
    delay(checkDelay);
  }

  robot->stopMotors();
  loop();
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
    robot->turnByAngle(90 - bestAngle);
  }
}

void amInWall()
{
  if (mapping)
  {
    int lidarValue = robot->get_lidar();

    if (lidarValue < stopDistance && lidarValue > 0)
    {
      robot->stopMotors();

      if (!firstWall)
      {
        robot->sendMapData();
      }

      if (firstWall)
      {
        measureWallAngle();
        delay(1000);
        firstWall = false;
      }

      robot->turnByAngle(-95);
      robot->countTurnAngle(-90);

      communicateWithPhone();
      robot->displayData();

      delay(500);
      rideAlongWall();
    }
  }
  else
  {
    loop();
  }
}

void setup()
{
  robot = new Robot();
}

void loop()
{
  communicateWithPhone();
  robot->displayData(); //send data and read incoming data

  if (mapping)
  {
    robot->bothMotorSpeed(robotSpeed);
    amInWall();
  }
  else
  {
    robot->stopMotors();
  }
}