#pragma once

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Servo.h>


class Robot {

public:
    const int temperatureSensorPin = 23;
    const int buzzerPin = 32;
    const int batteryPin = 2;
    const int servoPin = 15;


    //diplay parameters
    const int screenWidth = 128; // OLED display width, in pixels
    const int screenHeight = 64; // OLED display height, in pixels

    #define OLED_RESET -1
    #define SCREEN_ADDRESS 0x3C
    Adafruit_SSD1306 *display = new Adafruit_SSD1306(screenWidth, screenHeight, &Wire, OLED_RESET);

    //Dallas Temperature sensor
    OneWire *oneWire = new OneWire(temperatureSensorPin);
    DallasTemperature *sensor = new DallasTemperature(oneWire);

    //LiDar Servo
    Servo servo;
    int servoPos = 90;
    const int servoMin = 10;
    const int servoMax = 170;


    Robot();
    float readTemp();

    float readBatteryStatus();

    void soundEnd();

    void soundNote(note_t note, uint8_t octave);
    void soundBootUp();
    void displayBegin();

    int get_servoPos();
    void set_servoPos(int pos);
    void servoBegin();


private:
    const float batteryMax = 3150.0;

};