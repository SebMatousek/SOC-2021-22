#include "Robot.h"
#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Servo.h>
#include <ESP32MotorControl.h>
#include <ESP32Encoder.h>

void Robot::delete_enc_value(int motor)
{
    if(motor == 0)
    {
        encoder0->clearCount();
    }
    else
    {
        encoder1->clearCount();
    }
}

int Robot::get_enc_value(int motor)
{
    if(motor == 0)
    {
        return encoder0->getCount();
    }
    else
    {
        return encoder1->getCount();
    }
}

int Robot::get_motor_speed(int motor)
{
    return MotorControl.getMotorSpeed(motor);
}

void Robot::motorSpeed(int motor, int speed)
{   
    if(speed > 0 && speed <= 100)
    {
        MotorControl.motorForward(motor, speed);
    }
    else if(speed < 0 && speed >= -100)
    {
        MotorControl.motorReverse(motor, speed);
    }
    else if(speed == 0)
    {
        MotorControl.motorStop(motor);
    }
    else
    {   
        MotorControl.motorStop(motor);
        Serial.println("Not a valid motor speed!");
    }
}

void Robot::servoBegin()
{
    servo.attach(servoPin);
    servo.write(90);
}

int Robot::get_servoPos()
{
    return servoPos;
}

void Robot::set_servoPos(int pos)
{
    if(pos >= servoMin && pos <= servoMax)
    {
        servo.write(pos);
        servoPos = pos;
    }
}

void Robot::displayBegin()
{
    display->clearDisplay();

    display->setTextSize(4); // Draw 2X-scale text
    display->setTextColor(SSD1306_WHITE);
    display->setCursor(15, 20);
    display->println("BORR");
    display->display();
}

float Robot::readTemp()
{
    sensor->requestTemperatures(); 
    float temperatureC = sensor->getTempCByIndex(0);
    return temperatureC;
}

float Robot::readBatteryStatus()
{
    return (analogRead(batteryPin) / batteryMax) * 100.0;
}

void Robot::soundEnd() {
    ledcDetachPin(buzzerPin);
}

void Robot::soundNote(note_t note, uint8_t octave)
{
    ledcAttachPin(buzzerPin, 1);
    ledcWriteNote(1, note, octave);
}

void Robot::soundBootUp()
{
    soundNote(NOTE_C, 5);
    delay(150);
    soundEnd();
    soundNote(NOTE_G, 5);
    delay(500);
    soundEnd();
    delay(200);
}

Robot::Robot()
{
    Wire.begin();
    display->begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);

    Serial.begin(115200);

    pinMode(buzzerPin, OUTPUT);

    pinMode(in1, OUTPUT);
    pinMode(in2, OUTPUT);
    pinMode(in3, OUTPUT);
    pinMode(in4, OUTPUT);
    MotorControl.attachMotors(in2, in1, in3, in4);

    sensor->begin();

    displayBegin();
    soundBootUp();
    display->clear();

    servoBegin();

    ESP32Encoder::useInternalWeakPullResistors=UP;
    encoder0->attachHalfQuad(34, 35);
    encoder1->attachHalfQuad(4, 19);
    encoder0->clearCount();
    encoder1->clearCount();
}