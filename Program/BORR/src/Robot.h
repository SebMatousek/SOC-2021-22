#pragma once

#include <Arduino.h>

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

    void set_motor_speed(int motor, int speed);
    int get_motor_speed(int motor);

    void set_motor_both_speed(int speed);

    void motor_stop(int motor);
    void motor_both_stop();

    void turnByAngle(int angle);

    void turnWheels(float rotations0, float rotations1);

    void set_servo_position(int pos);
    int get_servo_position();

    int get_lidar_distance();

    int get_aku_percentage();
    int get_dallas_temperature();

    int get_enc_value(int enc);

    void delete_enc_value(int motor);
    void delete_both_enc_value();

    void buzzer_sound_WiFi_connected();

    void screenClear();

    void screen_display_data(int stepper);
    String decodeRobotName();

    //const String ROBOT_COLOR = "#Green";
    const String ROBOT_COLOR = "#Pink";
    //const String ROBOT_COLOR = "#Blue";
    //const String ROBOT_COLOR = "#Golden";

    int revolutionClicks = 4172;

private:

    const float batteryMax = 3150.0;

    const int temperatureSensorPin = 23;
    const int buzzerPin = 32;
    const int batteryPin = 36;
    const int servoPin = 15;

    // motor controller
    ESP32MotorControl MotorControl = ESP32MotorControl();
    const int in1 = 33;
    const int in2 = 25;
    const int in3 = 26;
    const int in4 = 27;

    // diplay parameters
    Display *screen = new Display();

    // Dallas Temperature sensor
    OneWire *oneWire = new OneWire(temperatureSensorPin);
    DallasTemperature *sensor = new DallasTemperature(oneWire);

    // LiDar Servo
    Servo servo;
    int servoPos = -1;

    ESP32Encoder *encoder0 = new ESP32Encoder();
    ESP32Encoder *encoder1 = new ESP32Encoder();

    //LiDAR
    Adafruit_VL53L0X *lidar = new Adafruit_VL53L0X();

    int lastStepper = -1;

    void motor_both_begin();
    void screen_begin();
    void servo_begin();
    void encoder_both_begin();
    void lidar_begin();
    void buzzer_begin();
    void dallas_begin();

    void buzzer_stop();
    void buzzer_sound_note(note_t note, uint8_t octave);
};