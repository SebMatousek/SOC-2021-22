#pragma once

#include <Arduino.h>

#include <WiFi.h>
#include <ESP32MotorControl.h>
#include <ESP32Encoder.h>
#include <Servo.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Adafruit_VL53L0X.h>
#include <Display.h>
#include <Wire.h>
#include <Adafruit_MPU6050.h>


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

    void turnByAngle(int angle);

    int get_enc_value(int motor);
    void delete_enc_value(int motor);

    int get_lidar();

    void readGyro();

    void sendSensorData();

    String getSensorData();
    String readData();

    void countTurnAngle(int angle);
    void sendMapData();

    void displayData();

    const int servoMin = 0;
    const int servoMax = 180;
    const int revolutionClicks = 4172;

    sensors_event_t a, g, temp;

private:

    //const String ROBOT_COLOR = "#Green";
    //const String ROBOT_COLOR = "#Pink";
    const String ROBOT_COLOR = "#Blue";
    //const String ROBOT_COLOR = "#Golden";

    const float batteryMax = 3150.0;

    const int temperatureSensorPin = 23;
    const int buzzerPin = 32;
    const int batteryPin = 36;
    const int servoPin = 15;


    // diplay parameters
    const int screenWidth = 128; // OLED display width, in pixels
    const int screenHeight = 64; // OLED display height, in pixels

    #define OLED_RESET -1
    #define SCREEN_ADDRESS 0x3C
    Display *screen = new Display();

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

    ESP32Encoder *encoder0 = new ESP32Encoder();
    ESP32Encoder *encoder1 = new ESP32Encoder();

    //LiDAR
    Adafruit_VL53L0X *lidar = new Adafruit_VL53L0X();

    Adafruit_MPU6050 mpu;

    const char* ssid = "Matousek";
    const char* password =  "Kokorin12";

    const uint16_t port = 8090;
    const char * host = "10.0.0.33";

    WiFiClient client;

    int angleTurned = 0;

    void soundBootUp();
    void displayBegin();
    void servoBegin();
    void beginGyro();
    void wifiBegin();
    void soundConnected();
    void soundDataSent();
};