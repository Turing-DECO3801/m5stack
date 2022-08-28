#include <WiFi.h>
#include <HTTPClient.h>
#include <M5Core2.h>

const char* ssid = "Elevate-41Warren";
const char* password = "Dyingfromexams";

String serverName = "https://2i5agd6rwacftajs3skgtcrlne0vqcgc.lambda-url.ap-southeast-2.on.aws/";

unsigned long lastTime = 0;

unsigned long timerDelay = 5000;

void setup() {
  M5.begin();
  Serial.begin(115200); 

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
 
  Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");
}


void loop() {
  //Send an HTTP POST request every 10 minutes
  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      WiFiClient client;
      HTTPClient http;
    
      // Your Domain name with URL path or IP address with path
      http.begin(serverName);

      //If you need an HTTP request with a content type: text/plain
      http.addHeader("Content-Type", "text/plain");
      int httpResponseCode = http.POST("lat,lng,time,dat\r\nlat2,lng2,time2,dat2\r\nlat,lng,time,dat\r\nlat2,lng2,time2,dat2\r\nlat,lng,time,dat\r\nlat2,lng2,time2,dat2\r\nlat,lng,time,dat\r\nlat2,lng2,time2,dat2\r\nlat,lng,time,dat\r\nlat2,lng2,time2,dat2\r\nlat,lng,time,dat\r\nlat2,lng2,time2,dat2\r\nlat,lng,time,dat\r\nlat2,lng2,time2,dat2\r\nlat,lng,time,dat\r\nlat2,lng2,time2,dat2\r\nlat,lng,time,dat\r\nlat2,lng2,time2,dat2\r\nlat,lng,time,dat\r\nlat2,lng2,time2,dat2\r\nlat,lng,time,dat\r\nlat2,lng2,time2,dat2\r\nlat,lng,time,dat\r\nlat2,lng2,time2,dat2\r\nlat,lng,time,dat\r\nlat2,lng2,time2,dat2\r\nlat,lng,time,dat\r\nlat2,lng2,time2,dat2\r\nlat,lng,time,dat\r\nlat2,lng2,time2,dat2\r\nlat,lng,time,dat\r\nlat2,lng2,time2,dat2\r\nlat,lng,time,dat\r\nlat2,lng2,time2,dat2\r\nlat,lng,time,dat\r\nlat2,lng2,time2,dat2\r\nlat,lng,time,dat\r\nlat2,lng2,time2,dat2\r\nlat,lng,time,dat\r\nlat2,lng2,time2,dat2\r\nlat,lng,time,dat\r\nlat2,lng2,time2,dat2\r\nlat,lng,time,dat\r\nlat2,lng2,time2,dat2\r\nlat,lng,time,dat\r\nlat2,lng2,time2,dat2\r\nlat,lng,time,dat\r\nlat2,lng2,time2,dat2\r\nlat,lng,time,dat\r\nlat2,lng2,time2,dat2\r\nlat,lng,time,dat\r\nlat2,lng2,time2,dat2\r\nlat,lng,time,dat\r\nlat2,lng2,time2,dat2\r\nlat,lng,time,dat\r\nlat2,lng2,time2,dat2\r\nlat,lng,time,dat\r\nlat2,lng2,time2,dat2\r\nlat,lng,time,dat\r\nlat2,lng2,time2,dat2\r\nlat,lng,time,dat\r\nlat2,lng2,time2,dat2\r\nlat,lng,time,dat\r\nlat2,lng2,time2,dat2\r\nlat,lng,time,dat\r\nlat2,lng2,time2,dat2\r\nlat,lng,time,dat\r\nlat2,lng2,time2,dat2\r\nlat,lng,time,dat\r\nlat2,lng2,time2,dat2\r\nlat,lng,time,dat\r\nlat2,lng2,time2,dat2\r\nlat,lng,time,dat\r\nlat2,lng2,time2,dat2\r\nlat,lng,time,dat\r\nlat2,lng2,time2,dat2\r\nlat,lng,time,dat\r\nlat2,lng2,time2,dat2\r\nlat,lng,time,dat\r\nlat2,lng2,time2,dat2\r\nlat,lng,time,dat\r\nlat2,lng2,time2,dat2\r\nlat,lng,time,dat\r\nlat2,lng2,time2,dat2\r\nlat,lng,time,dat\r\nlat2,lng2,time2,dat2\r\nlat,lng,time,dat\r\nlat2,lng2,time2,dat2\r\nlat,lng,time,dat\r\nlat2,lng2,time2,dat2\r\nlat,lng,time,dat\r\nlat2,lng2,time2,dat2\r\nlat,lng,time,dat\r\nlat2,lng2,time2,dat2\r\nlat,lng,time,dat\r\nlat2,lng2,time2,dat2\r\nlat,lng,time,dat\r\nlat2,lng2,time2,dat2\r\nlat,lng,time,dat\r\nlat2,lng2,time2,dat2\r\nlat,lng,time,dat\r\nlat2,lng2,time2,dat2\r\n");
     
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
        
      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}
