#include "Robot.h"
#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Servo.h>

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

    sensor->begin();

    displayBegin();
    soundBootUp();
    display->clear();

    servoBegin();
}