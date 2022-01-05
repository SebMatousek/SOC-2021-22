#include <WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>
 
const char* ssid = "Matousek";
const char* password =  "Kokorin12";
 
const uint16_t port = 8090;
const char * host = "10.0.0.33";

OneWire *oneWire = new OneWire(23);
DallasTemperature *sensor = new DallasTemperature(oneWire);

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

 client.print("#Pink");
 
}
 
void loop()
{
    sensor->requestTemperatures();
    int temperatureC = int(sensor->getTempCByIndex(0) * 10);
    

    int bat = (analogRead(36) / 196.0) * 100;
    Serial.println(analogRead(36));
    Serial.println(analogRead(36) / 196.0);
    Serial.println((analogRead(36) / 196.0) * 100);
    
    client.print(String(temperatureC) + "%" + String(bat) + "*");
    
    delay(1000);
}
