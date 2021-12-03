#include "Robot.h"

int Robot::get_lidar()
{
    VL53L0X_RangingMeasurementData_t measure;

    lidar->rangingTest(&measure, false);

    if(measure.RangeStatus != 4)
    {
        return measure.RangeMilliMeter;
    }
    else return -1;
}

void Robot::turn90(String direction)
{
    delete_enc_value(0);
    delete_enc_value(1);

    bool leftTurned = false;
    bool rightTurned = false;

    if (direction == "left")
    {
        motorSpeed(0, -100);
        motorSpeed(1, 100);
    }
    else if (direction == "right")
    {
        motorSpeed(0, 100);
        motorSpeed(1, -100);
    }

    while (!leftTurned || !rightTurned)
    {

        Serial.println("left... " + String(get_enc_value(0)));
        Serial.println("right... " + String(get_enc_value(1)));

        if (abs(get_enc_value(0)) > abs(revolutionClicks * 2/5))
        {
            stopMotor(0);
            leftTurned = true;
            
        }
        if (abs(get_enc_value(1)) > abs(revolutionClicks * 2/5))
        {
            stopMotor(1);
            rightTurned = true;
        }
    }
}

void Robot::stopMotors()
{
    MotorControl.motorsStop();
}

void Robot::stopMotor(int motor)
{
    MotorControl.motorStop(motor);
}

void Robot::turnWheel(int motor, float rotations)
{
    delete_enc_value(motor);
    bool turned = false;

    if(rotations > 0)
    {
        motorSpeed(motor, 100);
    }
    else if(rotations < 0)
    {
        motorSpeed(motor, -100);
    }

    while (!turned)
    {
        if (abs(get_enc_value(motor)) > abs(revolutionClicks * rotations))
        {
            turned = true;
            stopMotor(motor);
        }
    }
}

void Robot::turnWheels(float rotations0, float rotations1)
{
    delete_enc_value(0);
    delete_enc_value(1);

    bool leftTurned = false;
    bool rightTurned = false;

    if(rotations0 > 0)
    {
        motorSpeed(0, 100);
    }
    else if(rotations0 < 0)
    {
        motorSpeed(0, -100);
    }

    if(rotations1 > 0)
    {
        motorSpeed(1, 100);
    }
    else if(rotations1 < 0)
    {
        motorSpeed(1, -100);
    }

    while (!leftTurned || !rightTurned)
    {
        if (abs(get_enc_value(0)) > abs(revolutionClicks * rotations0))
        {
            stopMotor(0);
            leftTurned = true;
            
        }
        if (abs(get_enc_value(1)) > abs(revolutionClicks * rotations1))
        {
            stopMotor(1);
            rightTurned = true;
        }
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

int Robot::get_enc_value(int motor)
{
    if (motor == 0)
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

void Robot::bothMotorSpeed(int speed)
{
    if (speed > 0 && speed <= 100)
    {
        MotorControl.motorForward(0, speed);
        MotorControl.motorForward(1, speed);
    }
    else if (speed < 0 && speed >= -100)
    {
        MotorControl.motorForward(0, speed);
        MotorControl.motorForward(1, speed);
    }
    else if (speed == 0)
    {
        MotorControl.motorStop(0);
        MotorControl.motorStop(1);
    }
    else
    {
        MotorControl.motorStop(0);
        MotorControl.motorStop(1);
        Serial.println("Not a valid motor speed!");
    }
}

void Robot::motorSpeed(int motor, int speed)
{
    if (speed > 0 && speed <= 100)
    {
        MotorControl.motorForward(motor, speed);
    }
    else if (speed < 0 && speed >= -100)
    {
        MotorControl.motorReverse(motor, speed);
    }
    else if (speed == 0)
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
    if (pos >= servoMin && pos <= servoMax)
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

void Robot::soundEnd()
{
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

    ESP32Encoder::useInternalWeakPullResistors = UP;
    encoder0->attachHalfQuad(34, 35);
    encoder1->attachHalfQuad(4, 19);
    encoder0->clearCount();
    encoder1->clearCount();

    lidar->begin();
}