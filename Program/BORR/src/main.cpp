#include <Arduino.h>
#include <WiFi.h>

#include "Robot.h"

#define CAMERA

const char *ssid = "Matousek";      // Put your SSID here
const char *password = "Kokorin12"; // Put your PASSWORD here

#ifdef CAMERA
#include <Wire.h>
#include <ESP32WebServer.h>
#include <ArduCAM.h>
#include <SPI.h>
#include "memorysaver.h"

// set GPIO17 as the slave select :
const int CS = 16;

ArduCAM myCAM(OV5642, CS);

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
  WiFiClient serverClient = server.client();
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

  if (!serverClient.connected())
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
      if (!serverClient.connected())
        break;

      serverClient.write(&buffer[0], i);

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
        if (!serverClient.connected())
          break;

        serverClient.write(&buffer[0], bufferSize);

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
  //Serial.println(message);

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

  // Start the server
  server.on("/capture", HTTP_GET, serverCapture);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println(F("Server started"));
}
#endif

TaskHandle_t robotTask;
TaskHandle_t networkTask;

Robot *robot;

const uint16_t port = 8090;
const char * host = "10.0.0.33";

WiFiClient client;

int motor_speed = 100;
bool mapping = false;
int stepper = 0;

long long dataTimer = 0;
int dataTimeDelay = 1000;
long long speedTimer = 0;

unsigned int stopDistance = 120;
const unsigned int motorSpeedCushion = 15;
const unsigned int distanceCushion = 6;
int lastDistance = stopDistance / 2;
int angleTurned = 0;

long distanceTraveled = 0;

bool joystickEnabled = false;
bool wasMapping = false;
bool firstEnable = true;

void sendMapData()
{
    int distance = int(robot->get_enc_value(0) / float(robot->revolutionClicks) * 3.14 * 3.5);
    int speed = distance / (millis() - speedTimer) * 1000;
    distanceTraveled += distance;
    client.print("M%" + String(angleTurned) + "%" + String(distance) + "%" + String(distanceTraveled) + "%" + String(speed) + "*");
    robot->delete_both_enc_value();

    speedTimer = millis();
}

void countTurnAngle(int angle)
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

void send_data_sensors()
{
  if(millis() > dataTimer + dataTimeDelay)
  {
    client.print(String(robot->get_dallas_temperature() * 10) + "%" + String(robot->get_aku_percentage()) + "%" + "0%0*");
    dataTimer = millis();
  }
}

void recieve_data()
{
  if(client.available())
  {
    String message = client.readString();
    Serial.println(message);

    if(message.indexOf("map" >= 0))
    {
      Serial.println("start mapping");
      mapping = true;
    }
    if(message.indexOf("pause") >= 0)
    {
      Serial.println("pause mapping");
      mapping = false;
    }
    if(message.indexOf("disableJoystick") >= 0)
    {
      Serial.println("disable j");
      if(robot->ROBOT_COLOR == "#Golden")
      {
        joystickEnabled = false;
        mapping = wasMapping;
        firstEnable = true;
      }
    }
    if(message.indexOf("enableJoystick") >= 0)
    {
      Serial.println("enable j");
      if(robot->ROBOT_COLOR == "#Golden")
      {
        joystickEnabled = true;
        stepper = 0;
        wasMapping = mapping;
        mapping = false;
        firstEnable = true;
      }
    }

    if(joystickEnabled)
    {
      int direction = message.toInt();

      //Serial.println(message);

      if(direction % 10 == 0)
      {
        direction = 0;
      }

      switch(direction)
      {
        case 0:
          robot->motor_both_stop();
          break;
        case 1:
          robot->set_motor_both_speed(motor_speed);
          break;
        case 2:
          robot->set_motor_speed(0, motor_speed);
          robot->set_motor_speed(1, motor_speed / 1.5);
          break;
        case 3:
          robot->set_motor_speed(0, motor_speed);
          robot->set_motor_speed(1, -motor_speed);
          break;
        case 4:
          robot->set_motor_speed(0, -motor_speed);
          robot->set_motor_speed(1, -(motor_speed / 1.5));
          break;
        case 5:
          robot->set_motor_both_speed(-motor_speed);
          break;
        case 6:
          robot->set_motor_speed(0, -(motor_speed / 1.5));
          robot->set_motor_speed(1, -motor_speed);
          break;
        case 7:
          robot->set_motor_speed(0, -motor_speed);
          robot->set_motor_speed(1, motor_speed);
          break;
        case 8:
          robot->set_motor_speed(0, motor_speed / 1.5);
          robot->set_motor_speed(1, motor_speed);
          break;
      }
    }

    client.print("@Got the message: " + message);
  }
}

bool begin_WiFi()
{
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("...");
  }
 
  Serial.print("WiFi connected with IP: ");
  Serial.println(WiFi.localIP());

  if (!client.connect(host, port)) 
  {
        Serial.println("Connection to host failed");
        return false;
  }

  client.print(robot->ROBOT_COLOR);

  return true;
}

void getToWall()
{
  Serial.println("get to wall");
  speedTimer = millis();

  robot->set_motor_both_speed(motor_speed);

  int lidar_dist = robot->get_lidar_distance();

  while(lidar_dist > stopDistance || lidar_dist < 10)
  {
    lidar_dist = robot->get_lidar_distance();

    //Serial.println(lidar_dist);

    delay(20);
  }

  robot->motor_both_stop();
  stepper = 1;
}

void alignWithWall()
{
  Serial.println("align with wall");
  int wallDistances[18];

  for (int i = 0; i < 180; i += 10)
  {
    robot->set_servo_position(i);
    delay(150);
    wallDistances[i / 10] = robot->get_lidar_distance();
    delay(10);
  }

  int smallestDistance = 10000;
  int bestAngle = 0;

  for (int i = 0; i <= 18; i++)
  {
    //Serial.println(wallDistances[i]);
    if (smallestDistance > wallDistances[i] && wallDistances[i] > 0)
    {
      smallestDistance = wallDistances[i];
      bestAngle = i + 1 * 10;
    }
  }

  //Serial.println(bestAngle);

  robot->turnByAngle(90 - bestAngle -95);

  stepper = 2;
}

void checkSide()
{
  Serial.println("check side");
  robot->set_servo_position(0);

  delay(160);

  int lidarValue = robot->get_lidar_distance();

  if (lidarValue > lastDistance + stopDistance && lastDistance > 0)
  {
    robot->turnWheels(1.05, 1.05);

    sendMapData();

    robot->turnByAngle(100);
    countTurnAngle(90);

    delay(165);

    robot->set_motor_both_speed(motor_speed);
    lastDistance = stopDistance / 2;

    delay(75);
  }
  else if (lidarValue > stopDistance + distanceCushion)
  {
    if (robot->get_motor_speed(1) > 55 + motorSpeedCushion)
    {
      robot->set_motor_speed(1, robot->get_motor_speed(1) - motorSpeedCushion);
    }

    //Serial.println("to far");
  }
  else if (lidarValue < stopDistance - distanceCushion)
  {
    if (robot->get_motor_speed(0) > 55 + motorSpeedCushion)
    {
      robot->set_motor_speed(0, robot->get_motor_speed(0) - motorSpeedCushion);
    }

    //Serial.println("to close");
  }
  else
  {
    //Serial.println("sweetspot");
    robot->set_motor_both_speed(motor_speed);
  }

  lastDistance = lidarValue;
}

bool checkFront()
{
  Serial.println("check front");
  robot->set_servo_position(95);

  delay(160);

  int lidarValue = robot->get_lidar_distance();

  if (lidarValue < stopDistance && lidarValue > 0 && lidarValue < 8000)
  {
    return false;
  }

  return true;
}

void rideAlongWall()
{
  Serial.println("ride along wall");
  robot->set_motor_both_speed(motor_speed);
  
  while(checkFront() && !joystickEnabled)
  {
    checkSide();
  }
  if(!joystickEnabled)
  {
    robot->motor_both_stop();

    sendMapData();

    robot->turnByAngle(-90);
    countTurnAngle(-90);

    robot->set_motor_both_speed(motor_speed);
  }
  else
  {
    robot->motor_both_stop();
  }
}

//Task1code: blinks an LED every 1000 ms
void runRobot(void *pvParameters)
{
  Serial.print("robot control task running on core ");
  Serial.println(xPortGetCoreID());

  robot->screenClear();
  delay(300);

  for (;;)
  {
    robot->screen_display_data(stepper);

    if(mapping)
    {
      switch(stepper)
      {
        case 0:
          getToWall();
          break;
        case 1:
          alignWithWall();
          break;
        case 2:
          rideAlongWall();
          break;
      }
    }

    if(joystickEnabled && firstEnable)
    {
      client.print("!joystickEnabled*");
      Serial.println("j enabled!!");
      firstEnable = false;
    }

    delay(10);
  }
}


//Task2code: blinks an LED every 700 ms
void runNetwork(void *pvParameters)
{
  Serial.print("network task running on core ");
  Serial.println(xPortGetCoreID());

  if(!begin_WiFi())
  {
    Serial.println("WiFi didn't connect!");
    for(;;){delay(10);}
  }

  robot->buzzer_sound_WiFi_connected();
  robot->screen_display_data(stepper);

  #ifdef CAMERA
    setupCamera();
  #endif

  for (;;)
  {
    send_data_sensors();

    recieve_data();

    #ifdef CAMERA
        server.handleClient();
    #endif
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