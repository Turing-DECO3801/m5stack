#include <M5Core2.h>
#include <TinyGPSPlus.h>

#include "secrets.h"
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "WiFi.h"
 
#define AWS_IOT_PUBLISH_TOPIC   "esp32/pub"
#define AWS_IOT_SUBSCRIBE_TOPIC "esp32/sub"
 
float lat;
float lng;
char tme[32];
char dte[32];

WiFiClientSecure net = WiFiClientSecure();
PubSubClient client(net);

static const uint32_t GPSBaud = 9600;

//Creat The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
HardwareSerial ss(2);

void setup() {
  M5.begin();

  Serial.begin(115200);
  connectAWS();

  ss.begin(GPSBaud, SERIAL_8N1, 33, 32); 
}

void connectAWS()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
 
  Serial.println("Connecting to Wi-Fi");
 
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
 
  // Configure WiFiClientSecure to use the AWS IoT device credentials
  net.setCACert(AWS_CERT_CA);
  net.setCertificate(AWS_CERT_CRT);
  net.setPrivateKey(AWS_CERT_PRIVATE);
 
  // Connect to the MQTT broker on the AWS endpoint we defined earlier
  client.setServer(AWS_IOT_ENDPOINT, 8883);
 
  // Create a message handler
  client.setCallback(messageHandler);
 
  Serial.println("Connecting to AWS IOT");
 
  while (!client.connect(THINGNAME))
  {
    Serial.print(".");
    delay(100);
  }
 
  if (!client.connected())
  {
    Serial.println("AWS IoT Timeout!");
    return;
  }
 
  // Subscribe to a topic
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);
 
  Serial.println("AWS IoT Connected!");
}

/* Function called to publish JSON message to MTQQ  */
void publishMessage()
{
  StaticJsonDocument<200> doc;
  doc["latitude"] = lat;
  doc["longitude"] = lng;
  doc["date"] = dte;
  doc["time"] = tme;
  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer); // print to client
 
  client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
}

/* Empty function to set up client */
void messageHandler(char* topic, byte* payload, unsigned int length)
{
  return;
}

int buttonA_pressed = 0;

long int last_time = millis();

void loop() {
  M5.update();

  if (M5.BtnA.wasReleased() || M5.BtnA.pressedFor(0, 200)) {
    M5.Axp.SetLDOEnable(3, true);  //Open the vibration.   开启震动马达
    if (buttonA_pressed == 0) {
      updateValues();
      publishMessage();
    }
    buttonA_pressed = 1;
    last_time = millis();
  }
  
  if (last_time + 200 < millis()) {
    M5.Axp.SetLDOEnable(3, false);  //Open the vibration.   开启震动马达
    buttonA_pressed = 0;
  }
  
  client.loop();
}

/* Reads new GPS values and updates their variables for publishing */
void updateValues() {
  TinyGPSDate d = gps.date;
  TinyGPSTime t = gps.time;
  smartDelay(0);
  lat = gps.location.lat();
  smartDelay(0);
  lng = gps.location.lng();
  smartDelay(0);
  sprintf(dte, "%02d/%02d/%02d", d.month(), d.day(), d.year());
  sprintf(tme, "%02d:%02d:%02d", t.hour(), t.minute(), t.second());
}

/* Custom version of delay which waits for GPS data to be read from serial input */
static void smartDelay(unsigned long ms) {
  unsigned long start = millis();
  do { 
    while (ss.available()) gps.encode(ss.read());
  } while (millis() - start < ms);
}
