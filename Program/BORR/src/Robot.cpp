#include "Robot.h"

String Robot::readData()
{
    if (client.available())
    {
        //Serial.println(client.readString());
        return client.readString();
    }
    else
    {
        return "-";
    }
}

void Robot::soundDataSent()
{
    soundNote(NOTE_B, 5);
    delay(250);
    soundNote(NOTE_B, 4);
    delay(250);
    soundEnd();
    delay(500);
}

void Robot::sendSensorData()
{
    client.print(getSensorData());
}

String Robot::getSensorData()
{
    return String(int(readTemp() * 10)) + "%" + String(int(readBatteryStatus())) + "*";
}

void Robot::sendMapData()
{
    client.print("M%" + String(angleTurned) + "%" + String(int((get_enc_value(0) / float(revolutionClicks) * 3.14 * 3.5) * 100)) + "%" + String(0) + "*");
    delete_enc_value(0);
    delete_enc_value(1);
}

void Robot::countTurnAngle(int angle)
{
    angleTurned += angle;

    if(angleTurned == 360 || angleTurned == -360)
    { 
        angleTurned = 0;
    }
    if(angleTurned == -270)
    {
        angleTurned = 90;
    }
    if(angleTurned == 270)
    {
        angleTurned = -90;
    }
}

void Robot::soundConnected()
{
    soundNote(NOTE_E, 5);
    delay(250);
    soundNote(NOTE_E, 5);
    delay(250);
    soundEnd();
    delay(500);
}

void Robot::wifiBegin()
{
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.println("...");
    }

    Serial.print("WiFi connected with IP: ");
    Serial.println(WiFi.localIP());

    if (!client.connect(host, port))
    {
        Serial.println("Connection to host failed");

        delay(1000);
        return;
    }

    client.print(ROBOT_COLOR);

    soundConnected();
}

void Robot::readGyro()
{
    mpu.getEvent(&a, &g, &temp);

    Serial.print("Acceleration X: ");
    Serial.print(a.acceleration.x);
    Serial.print(", Y: ");
    Serial.print(a.acceleration.y);
    Serial.print(", Z: ");
    Serial.print(a.acceleration.z);
    Serial.println(" m/s^2");

    Serial.print("Rotation X: ");
    Serial.print(g.gyro.x);
    Serial.print(", Y: ");
    Serial.print(g.gyro.y);
    Serial.print(", Z: ");
    Serial.print(g.gyro.z);
    Serial.println(" rad/s");

    Serial.println("");
}

void Robot::beginGyro()
{
    if (!mpu.begin())
    {
        Serial.println("Failed to find MPU6050 chip");
        while (1)
        {
            delay(10);
        }
    }
}

void Robot::turnByAngle(int angle)
{
    delete_enc_value(0);
    delete_enc_value(1);

    if (angle > 0)
    {
        motorSpeed(0, 50);
        motorSpeed(1, -50);
    }
    else
    {
        motorSpeed(0, -50);
        motorSpeed(1, 50);
    }

    bool finished0 = false;
    bool finished1 = false;

    int rotations0 = revolutionClicks * abs(angle / 180.0);
    int rotations1 = revolutionClicks * abs(angle / 180.0);

    //Serial.println(rotations0);
    //Serial.println(rotations1);

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

    stopMotors();
}

int Robot::get_lidar()
{
    VL53L0X_RangingMeasurementData_t measure;

    lidar->rangingTest(&measure, false);

    if (measure.RangeStatus != 4)
    {
        return measure.RangeMilliMeter;
    }
    else
        return -1;
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

        if (abs(get_enc_value(0)) > abs(revolutionClicks * 2 / 5))
        {
            stopMotor(0);
            leftTurned = true;
        }
        if (abs(get_enc_value(1)) > abs(revolutionClicks * 2 / 5))
        {
            stopMotor(1);
            rightTurned = true;
        }
    }
}

void Robot::stopMotors()
{
    stopMotor(0);
    stopMotor(1);
}

void Robot::stopMotor(int motor)
{
    if (MotorControl.getMotorSpeed(motor) != 0)
    {
        if (MotorControl.isMotorForward(motor))
        {
            motorSpeed(motor, -100);
        }
        else
        {
            motorSpeed(motor, 100);
        }

        delay(15);

        MotorControl.motorStop(motor);
    }
}

void Robot::turnWheel(int motor, float rotations)
{
    delete_enc_value(motor);
    bool turned = false;

    if (rotations > 0)
    {
        motorSpeed(motor, 100);
    }
    else if (rotations < 0)
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

    if (rotations0 > 0)
    {
        motorSpeed(0, 100);
    }
    else if (rotations0 < 0)
    {
        motorSpeed(0, -100);
    }

    if (rotations1 > 0)
    {
        motorSpeed(1, 100);
    }
    else if (rotations1 < 0)
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
    screen->clear();
    screen->println("Bezdratove", 0, 2);
    screen->println(" ovladany", 3, 2);
    screen->println("roj robotu", 5, 2);  
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

void Robot::displayData()
{
    screen->clear(0);
    screen->clear(1);
    screen->clear(2);
    screen->print("              " + String(readBatteryStatus()) + "%", 1, 1);
}

Robot::Robot()
{
    Wire.begin();

    displayBegin();
    
    soundBootUp();
    screen->clear();

    Serial.begin(115200);

    pinMode(buzzerPin, OUTPUT);

    pinMode(in1, OUTPUT);
    pinMode(in2, OUTPUT);
    pinMode(in3, OUTPUT);
    pinMode(in4, OUTPUT);
    MotorControl.attachMotors(in2, in1, in3, in4);

    sensor->begin();
    readTemp();

    ESP32Encoder::useInternalWeakPullResistors = UP;
    encoder0->attachHalfQuad(34, 35);
    encoder1->attachHalfQuad(4, 19);
    encoder0->clearCount();
    encoder1->clearCount();

    lidar->begin();

    wifiBegin();

    servoBegin();

    screen->print(ROBOT_COLOR.substring(1), 2.5, 3);
}