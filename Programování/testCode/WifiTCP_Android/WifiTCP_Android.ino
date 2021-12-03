#include <WiFi.h>
 
const char* ssid = "Matousek";
const char* password =  "Kokorin12";
 
const uint16_t port = 8090;
const char * host = "10.0.0.33";

WiFiClient client;

int buzz = 32;
 
void setup()
{
 
  Serial.begin(115200);
 
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("...");
  }
 
  Serial.print("WiFi connected with IP: ");
  Serial.println(WiFi.localIP());

  if (!client.connect(host, port)) {
 
        Serial.println("Connection to host failed");
 
        delay(1000);
        return;
    }

   pinMode(buzz, OUTPUT);
  digitalWrite(buzz, HIGH);
  delay(1000);
  digitalWrite(buzz, LOW);
 
}
 
void loop()
{

    client.print("Hello, esp 2 here");
    
  
    delay(1000);
}
