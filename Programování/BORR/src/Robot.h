#pragma once

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
#include <Adafruit_VL53L0X.h>

class Robot {

public:
    Robot();
    float readTemp();

    float readBatteryStatus();

    void soundEnd();

    void soundNote(note_t note, uint8_t octave);

    int get_servoPos();
    void set_servoPos(int pos);

    void motorSpeed(int motor, int speed);
    void bothMotorSpeed(int speed);
    int get_motor_speed(int motor);
    void turn90(String direction);
    void turnWheel(int motor, float rotations);
    void turnWheels(float rotations0, float rotations1);
    void stopMotor(int motor);
    void stopMotors();

    int get_enc_value(int motor);
    void delete_enc_value(int motor);

    int get_lidar();


    const int servoMin = 0;
    const int servoMax = 180;
    

private:
    const float batteryMax = 3150.0;

    const int temperatureSensorPin = 23;
    const int buzzerPin = 32;
    const int batteryPin = 2;
    const int servoPin = 15;


    // diplay parameters
    const int screenWidth = 128; // OLED display width, in pixels
    const int screenHeight = 64; // OLED display height, in pixels

    #define OLED_RESET -1
    #define SCREEN_ADDRESS 0x3C
    Adafruit_SSD1306 *display = new Adafruit_SSD1306(screenWidth, screenHeight, &Wire, OLED_RESET);

    // Dallas Temperature sensor
    OneWire *oneWire = new OneWire(temperatureSensorPin);
    DallasTemperature *sensor = new DallasTemperature(oneWire);

    // LiDar Servo
    Servo servo;
    int servoPos = 90;

    // motor controller
    ESP32MotorControl MotorControl = ESP32MotorControl();
    const int in1 = 33;
    const int in2 = 25;
    const int in3 = 26;
    const int in4 = 27;

    const int revolutionClicks = 4172;

    ESP32Encoder *encoder0 = new ESP32Encoder();
    ESP32Encoder *encoder1 = new ESP32Encoder();

    //LiDAR
    Adafruit_VL53L0X *lidar = new Adafruit_VL53L0X();

    void soundBootUp();
    void displayBegin();
    void servoBegin();
};