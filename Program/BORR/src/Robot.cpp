#include "Robot.h"

Robot::Robot()
{
    Wire.begin();

    motor_both_begin();
    encoder_both_begin();

    lidar_begin();
    dallas_begin();

    screen_begin();
    buzzer_begin();

    servo_begin();
}

//Motors
void Robot::motor_both_begin()
{
    pinMode(in1, OUTPUT);
    pinMode(in2, OUTPUT);
    pinMode(in3, OUTPUT);
    pinMode(in4, OUTPUT);
    MotorControl.attachMotors(in2, in1, in3, in4);
}

void Robot::set_motor_speed(int motor, int speed)
{
    if (speed > 0 && speed <= 100)
    {
        MotorControl.motorForward(motor, abs(speed));
    }
    else if (speed < 0 && speed >= -100)
    {
        MotorControl.motorReverse(motor, abs(speed));
    }
    else
    {
        MotorControl.motorStop(motor);
    }
}

int Robot::get_motor_speed(int motor)
{
    return MotorControl.getMotorSpeed(motor);
}

void Robot::set_motor_both_speed(int speed)
{
    set_motor_speed(0, speed);
    set_motor_speed(1, speed);
}

void Robot::motor_stop(int motor)
{
    if (MotorControl.getMotorSpeed(motor) != 0)
    {
        if (MotorControl.isMotorForward(motor))
        {
            set_motor_speed(motor, -100);
        }
        else
        {
            set_motor_speed(motor, 100);
        }

        delay(17);

        MotorControl.motorStop(motor);
    }
}

void Robot::motor_both_stop()
{
    motor_stop(0);
    motor_stop(1);
}

void Robot::turnByAngle(int angle)
{
    delete_both_enc_value();

    if (angle > 0)
    {
        set_motor_speed(0, 50);
        set_motor_speed(1, -50);
    }
    else if (angle != 0)
    {
        set_motor_speed(0, -50);
        set_motor_speed(1, 50);
    }

    bool finished0 = false;
    bool finished1 = false;

    int rotations0 = revolutionClicks * abs(angle / 180.0);
    int rotations1 = revolutionClicks * abs(angle / 180.0);

    while (!finished0 || !finished1)
    {
        if (abs(get_enc_value(0)) > rotations0)
        {
            finished0 = true;
        }
        if (abs(get_enc_value(1)) > rotations1)
        {
            finished1 = true;
        }
    }

    motor_both_stop();
}

void Robot::turnWheels(float rotations0, float rotations1)
{
    delete_both_enc_value();

    bool leftTurned = false;
    bool rightTurned = false;

    if (rotations0 > 0)
    {
        set_motor_speed(0, 100);
    }
    else if (rotations0 < 0)
    {
        set_motor_speed(0, -100);
    }

    if (rotations1 > 0)
    {
        set_motor_speed(1, 100);
    }
    else if (rotations1 < 0)
    {
        set_motor_speed(1, -100);
    }

    while (!leftTurned || !rightTurned)
    {
        if (abs(get_enc_value(0)) > abs(revolutionClicks * rotations0))
        {
            motor_stop(0);
            leftTurned = true;
        }
        if (abs(get_enc_value(1)) > abs(revolutionClicks * rotations1))
        {
            motor_stop(1);
            rightTurned = true;
        }
    }
}

//Encoders
void Robot::encoder_both_begin()
{
    ESP32Encoder::useInternalWeakPullResistors = UP;
    encoder0->attachHalfQuad(34, 35);
    encoder1->attachHalfQuad(4, 19);
    encoder0->clearCount();
    encoder1->clearCount();
}

int Robot::get_enc_value(int enc)
{
    if (enc == 0)
    {
        return encoder0->getCount();
    }
    else
    {
        return encoder1->getCount();
    }
}

void Robot::delete_enc_value(int motor)
{
    if (motor == 0)
    {
        encoder0->clearCount();
    }
    else
    {
        encoder1->clearCount();
    }
}
void Robot::delete_both_enc_value()
{
    delete_enc_value(0);
    delete_enc_value(1);
}

//Servo
void Robot::servo_begin()
{
    servo.attach(servoPin, 7);
    set_servo_position(90);
}

void Robot::set_servo_position(int pos)
{
    if (pos >= 0 && pos <= 180)
    {
        servo.write(pos);
        servoPos = pos;
    }
    else
    {
        Serial.println("wtf");
    }

    Serial.println(pos);
}
int Robot::get_servo_position()
{
    return servoPos;
}

//Lidar
void Robot::lidar_begin()
{
    lidar->begin();
}

int Robot::get_lidar_distance()
{
    VL53L0X_RangingMeasurementData_t measure;

    lidar->rangingTest(&measure, false);

    if (measure.RangeStatus != 4)
    {
        return measure.RangeMilliMeter;
    }
    else
    {
        return -1;
    }
}

//Screen
void Robot::screen_begin()
{
    screen->print("Bezdratove", 1, 2);
    screen->print(" ovladany ", 3, 2);
    screen->print("roj robotu", 5, 2);
}

void Robot::screenClear()
{
    screen->display_clear();
}

String Robot::decodeRobotName()
{
    if(ROBOT_COLOR == "#Green") return "Zeleny";
    if(ROBOT_COLOR == "#Pink") return "Ruzovy";
    if(ROBOT_COLOR == "#Blue") return "Modry ";
    if(ROBOT_COLOR == "#Golden") return "Zlaty ";
}

void Robot::screen_display_data(int stepper)
{
    if(stepper != lastStepper)
    {
        screen->display_clear();

        screen->print(decodeRobotName() + "         " + String(get_aku_percentage()) + "%", 1, 1, true);
        screen->print("   " + String(stepper), 3, 3, true);
    }

    lastStepper = stepper;
}

//Buzzer
void Robot::buzzer_begin()
{
    ledcSetup(5, 5000, 8);
    pinMode(buzzerPin, OUTPUT);

    buzzer_sound_note(NOTE_C, 5);
    delay(150);
    buzzer_sound_note(NOTE_G, 5);
    delay(500);
    buzzer_stop();
    delay(200);
}

void Robot::buzzer_sound_note(note_t note, uint8_t octave)
{
    ledcAttachPin(buzzerPin, 5);
    ledcWriteNote(5, note, octave);
}
void Robot::buzzer_stop()
{
    ledcDetachPin(buzzerPin);
}

void Robot::buzzer_sound_WiFi_connected()
{
    buzzer_sound_note(NOTE_E, 5);
    delay(250);
    buzzer_sound_note(NOTE_E, 5);
    delay(250);
    buzzer_stop();
    delay(500);
}

//Akumulator
int Robot::get_aku_percentage()
{
    return (analogRead(batteryPin) / batteryMax) * 100.0;
}

//Dallas temperature sensor
void Robot::dallas_begin()
{
    sensor->begin();
    get_dallas_temperature();
}

int Robot::get_dallas_temperature()
{
    sensor->requestTemperatures();
    float temperatureC = sensor->getTempCByIndex(0);

    return temperatureC;
}