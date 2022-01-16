#include <Arduino.h>
#include "Robot.h"

//#define CAMERA

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

int robotSpeed = 100;

int phase = 0;

#ifdef CAMERA
#include <Wire.h>
#include <ESP32WebServer.h>
#include <ArduCAM.h>
#include <SPI.h>
#include "memorysaver.h"

// set GPIO17 as the slave select :
const int CS = 16;

ArduCAM myCAM(OV5642, CS);

//Station mode you should put your ssid and password
const char *ssid = "Matousek";      // Put your SSID here
const char *password = "Kokorin12"; // Put your PASSWORD here

static const size_t bufferSize = 2048;
static uint8_t buffer[bufferSize] = {0xFF};
uint8_t temp = 0, temp_last = 0;
int i = 0;
bool is_header = false;

ESP32WebServer server(80);

void start_capture()
{
  myCAM.clear_fifo_flag();
  myCAM.start_capture();
}

void camCapture(ArduCAM myCAM)
{
  WiFiClient client = server.client();
  uint32_t len = myCAM.read_fifo_length();
  if (len >= MAX_FIFO_SIZE) //8M
  {
    Serial.println(F("Over size."));
  }
  if (len == 0) //0 kb
  {
    Serial.println(F("Size is 0."));
  }

  myCAM.CS_LOW();
  myCAM.set_fifo_burst();

  if (!client.connected())
    return;

  String response = "HTTP/1.1 200 OK\r\n";
  response += "Content-Type: image/jpeg\r\n";
  response += "Content-len: " + String(len) + "\r\n\r\n";
  server.sendContent(response);
  i = 0;

  while (len--)
  {
    temp_last = temp;
    temp = SPI.transfer(0x00);
    //Read JPEG data from FIFO
    if ((temp == 0xD9) && (temp_last == 0xFF)) //If find the end, break while,
    {
      buffer[i++] = temp; //save the last  0XD9
      //Write the remain bytes in the buffer
      if (!client.connected())
        break;

      client.write(&buffer[0], i);

      is_header = false;
      i = 0;
      myCAM.CS_HIGH();
      break;
    }
    if (is_header == true)
    {
      //Write image data to buffer if not full
      if (i < bufferSize)
        buffer[i++] = temp;
      else
      {
        //Write bufferSize bytes image data to file
        if (!client.connected())
          break;

        client.write(&buffer[0], bufferSize);

        i = 0;
        buffer[i++] = temp;
      }
    }
    else if ((temp == 0xD8) & (temp_last == 0xFF))
    {
      is_header = true;
      buffer[i++] = temp_last;
      buffer[i++] = temp;
    }
  }
}

void serverCapture()
{
  delay(1000);
  start_capture();

  while (!myCAM.get_bit(ARDUCHIP_TRIG, CAP_DONE_MASK))
    ;

  camCapture(myCAM);
}

void handleNotFound()
{
  String message = "Server is running!\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  server.send(200, "text/plain", message);
  Serial.println(message);

  if (server.hasArg("ql"))
  {
    int ql = server.arg("ql").toInt();

    myCAM.OV5642_set_JPEG_size(ql);

    Serial.println("QL change to: " + server.arg("ql"));
  }
}

void setupCamera()
{
  uint8_t vid, pid;
  uint8_t temp;
  //set the CS as an output:
  pinMode(CS, OUTPUT);

  Wire.begin();

  Serial.begin(115200);
  Serial.println(F("ArduCAM Start!"));

  // initialize SPI:
  SPI.begin(14, 12, 13, 16);
  SPI.setFrequency(4000000); //4MHz

  //Check if the ArduCAM SPI bus is OK
  myCAM.write_reg(ARDUCHIP_TEST1, 0x55);
  temp = myCAM.read_reg(ARDUCHIP_TEST1);
  if (temp != 0x55)
  {
    Serial.println(F("SPI1 interface Error!"));
    while (1)
      ;
  }

  myCAM.wrSensorReg16_8(0xff, 0x01);
  myCAM.rdSensorReg16_8(OV5642_CHIPID_HIGH, &vid);
  myCAM.rdSensorReg16_8(OV5642_CHIPID_LOW, &pid);
  if ((vid != 0x56) || (pid != 0x42))
  {
    Serial.println(F("Can't find OV5642 module!"));
  }
  else
    Serial.println(F("OV5642 detected."));

  //Change to JPEG capture mode and initialize the OV2640 module
  myCAM.set_format(JPEG);
  myCAM.InitCAM();

  myCAM.write_reg(ARDUCHIP_TIM, VSYNC_LEVEL_MASK); //VSYNC is active HIGH
  myCAM.OV5640_set_JPEG_size(OV5642_320x240);

  myCAM.clear_fifo_flag();

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print(F("Connecting to "));
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(F("."));
  }

  Serial.println(F("WiFi connected"));
  Serial.println("");
  Serial.println(WiFi.localIP());

  // Start the server
  server.on("/capture", HTTP_GET, serverCapture);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println(F("Server started"));
}

#endif

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
  readIncoming();

  sendSensorData();
  robot->sendMapData();
}

void checkSide()
{
  robot->set_servoPos(robot->servoMin);

  delay(150);

  int lidarValue = robot->get_lidar();

  if (lidarValue > lastDistance + stopDistance)
  {
    robot->turnWheels(1.25, 1.25);

    robot->turnByAngle(90);
    robot->countTurnAngle(90);

    delay(1000);
    
    while(!mapping){}

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
  robot->bothMotorSpeed(robotSpeed);

  while (checkForward() && mapping)
  {
    delay(checkDelay);
    
    if(!mapping)
    {
      int m0 = robot->get_motor_speed(0);
      int m1 = robot->get_motor_speed(1);

      robot->stopMotors();
      while(!mapping){}

      robot->motorSpeed(0, m0);
      robot->motorSpeed(1, m1);
    }

    checkSide();

    delay(checkDelay);
  }

  robot->stopMotors();

  while(!mapping){}

  amInWall();
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
    Serial.println(wallDistances[i]);
    if (smallestDistance > wallDistances[i] && wallDistances[i] > 0)
    {
      smallestDistance = wallDistances[i];
      bestAngle = i * 10;
    }
  }

  Serial.println(bestAngle);
  //robot->debugOverWiFi(String("best angle " + bestAngle));

  if (bestAngle != 90)
  {
    robot->turnByAngle(90 - bestAngle);
  }
}

void amInWall()
{
  int lidarValue = robot->get_lidar();
  //Serial.println(lidarValue); 

  if (lidarValue < stopDistance && lidarValue > 0)
  {
    robot->stopMotors();

    if (firstWall)
    {
      measureWallAngle();
      delay(1000);
      firstWall = false;
    }

    robot->turnByAngle(-95);
    robot->countTurnAngle(-90);

    delay(500);

    while(!mapping){}

    rideAlongWall();
  }
}

TaskHandle_t robotTask;
TaskHandle_t networkTask;

//Task1code: blinks an LED every 1000 ms
void runRobot(void *pvParameters)
{
  Serial.print("robot control task running on core ");
  Serial.println(xPortGetCoreID());
  delay(500);

  for (;;)
  {
    while(!mapping){delay(20);}

    robot->bothMotorSpeed(robotSpeed);
    amInWall();
  }
}


//Task2code: blinks an LED every 700 ms
void runNetwork(void *pvParameters)
{
  Serial.print("network task running on core ");
  Serial.println(xPortGetCoreID());

#ifdef CAMERA
  setupCamera();
#endif

  for (;;)
  {
    communicateWithPhone();

    #ifdef CAMERA
        server.handleClient();
    #endif

    delay(20);
  }
}

void setup()
{
  Serial.begin(115200);
  robot = new Robot();

  //create a task that will be executed in the Task1code() function, with priority 1 and executed on core 0
  xTaskCreatePinnedToCore(
      runRobot,   // Task function.
      "runRobot", // name of task.
      5000,       // Stack size of task
      NULL,       // parameter of the task
      1,          // priority of the task
      &robotTask, // Task handle to keep track of created task
      0);         // pin task to core 0 *
  delay(500);

  //create a task that will be executed in the Task2code() function, with priority 1 and executed on core 1
  xTaskCreatePinnedToCore(
      runNetwork,   /* Task function. */
      "runNetwork", /* name of task. */
      8000,         /* Stack size of task */
      NULL,         /* parameter of the task */
      1,            /* priority of the task */
      &networkTask, /* Task handle to keep track of created task */
      1);           /* pin task to core 1 */
  delay(500);
}

void loop()
{
}