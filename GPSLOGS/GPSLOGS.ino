// M5 Stack CORE2
#include <M5Core2.h>

// HTTP POST Request
#include <WiFi.h>
#include <HTTPClient.h>

// GPS Library
#include <TinyGPSPlus.h>

// File Storage Library
#include <SPIFFS.h>






// WIFI Credentials **SHOULD BE CHANGED TO WHATEVER THE USER'S DEFAULT SETTING IS**
const char* ssid = "Elevate-41Warren";
const char* password = "Dyingfromexams";

// Server that will receive the HTTP POST request
String serverName = "https://2i5agd6rwacftajs3skgtcrlne0vqcgc.lambda-url.ap-southeast-2.on.aws/";

// Location where GPS Logs will be stored for a single hike
String LOGS_LOCATION = "/logs.txt";

// The serial connection to the GPS device
HardwareSerial ss(2);

// GPS Baud Rate
static const uint32_t GPSBaud = 9600;

//TinyGPS++ Global Object for retrieval of GPS data
TinyGPSPlus gps;











void setup() {
  M5.begin();

  // Serial Debugging Setup
  Serial.begin(115200); 

  // GPS Setup
  ss.begin(GPSBaud, SERIAL_8N1, 33, 32);

  // SPIFFS Setup
  formatSPIFFS();
}




bool button_A_pressed = false;

bool button_B_pressed = false;

long int last_A_time = millis();

long int last_B_time = millis();

void loop() {
  M5.update();

  if (M5.BtnA.wasReleased() || M5.BtnA.pressedFor(0, 200)) {
    M5.Axp.SetLDOEnable(3, true);
    if (!button_A_pressed) {
      log_GPS_data();
    }
    button_A_pressed = true;
    last_A_time = millis();
  }

  if (M5.BtnB.wasReleased() || M5.BtnB.pressedFor(0, 200)) {
    M5.Axp.SetLDOEnable(3, true);
    if (!button_B_pressed) {
      public_HTTP_request();
    }
    button_B_pressed = true;
    last_B_time = millis();
  }



  if (last_A_time + 200 < millis()) {
    M5.Axp.SetLDOEnable(3, false);
    button_A_pressed = false;
  }
  
  if (last_B_time + 200 < millis()) {
    M5.Axp.SetLDOEnable(3, false);
    button_B_pressed = false;
  }
}






// Determines if a unique timestamp has been added
bool timestamp_added = false;

// Location for the storage of the timestamp
char timestamp[48];

// Latitude used for logs
float lat;

// Longitude used for logs
float lng;

// Current Time used for logs
char tme[16];

// Current Date used for logs
char dte[16];

// Global variable of the Object used for Reading and Writing to the Log file
File logs;

/**
 * Stores the GPS data in a text file for later retrieval and posting to AWS Lambda
 */
void log_GPS_data() {
  smartDelay(0);
  if (gps.location.isValid()) {
    smartDelay(0);
    TinyGPSDate d = gps.date;
    TinyGPSTime t = gps.time;
    
    smartDelay(0);
    lat = gps.location.lat();
    
    smartDelay(0);
    lng = gps.location.lng();
  
    
    sprintf(dte, "%02d-%02d-%02d", d.year(), d.day(), d.month());
    sprintf(tme, "%02d:%02d:%02d", t.hour(), t.minute(), t.second());
    if (!timestamp_added) {
      sprintf(timestamp, "%02d-%02d-%02d %02d:%02d:%02d", d.year(), d.day(), d.month(), t.hour(), t.minute(), t.second());
      timestamp_added = true;
    }
  
    append_to_logs();
    M5.Lcd.print("S");
  }
  M5.Lcd.print("A");
}

/**
 * Adds the updated GPS values to the file at location LOGS_LOCATION
 */
void append_to_logs() {
  logs = SPIFFS.open(LOGS_LOCATION, "a");

  char lineBuffer[30];
  sprintf(lineBuffer, "%f,%f,", lat, lng);
  logs.print(lineBuffer);
  logs.print(dte);
  logs.print(",");
  logs.print(tme);
  logs.print("\r\n");

  logs.close();
}

void print_log_data() {
  logs = SPIFFS.open(LOGS_LOCATION, "r");

  uint8_t* log_data = (uint8_t *)ps_calloc(logs.size() + 1, sizeof(char));
  int i = 0;
  for (i = 0; i < logs.size(); i++) {
    log_data[i] = logs.read();
  }
  log_data[i] = '\0';
  
  M5.Lcd.print((char*)log_data);

  logs.close();
}







/**
 * Formats the SPIFFS if not done so already, then it will attempt to open a new file
 */
void formatSPIFFS() {
  M5.Lcd.println("SPIFFS format start...");
  SPIFFS.format();  // Formatting SPIFFS
  M5.Lcd.println("SPIFFS format finish");

  SPIFFS.begin(true);

  // Clears the file to ensure that all logs are fresh
  if (SPIFFS.exists(LOGS_LOCATION)) {
    M5.Lcd.println("CLEARING EXISTING LOG FILES");
    SPIFFS.remove(LOGS_LOCATION);
  }

  // Create the file to be appended to
  logs = SPIFFS.open(LOGS_LOCATION, FILE_WRITE);
  M5.Lcd.println("CREATING NEW LOG FILES");
  logs.close();
}











// Flag to check that multiple attempts to connect to the Internet are not
// attempted to prevent duplication
volatile bool wifi_connection_attempted = false;

/**
 * Attempts to send a POST request through HTTP. If an internet connection has not yet
 * been established, it will keep attempting until it finds one. 
 * 
 * The POST request will send data for GPS Logs and the WAV sound files.
 */
void public_HTTP_request() {

  if (!wifi_connection_attempted) {
    WiFi.begin(ssid, password);
    wifi_connection_attempted = true;
    Serial.println("WIFI Connection Attempt Established");
  }

  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  if(WiFi.status()== WL_CONNECTED){
    HTTPClient http;
  
    // Your Domain name with URL path or IP address with path
    http.begin(serverName);

    //If you need an HTTP request with a content type: text/plain
    http.addHeader("Content-Type", "text/plain");


    

    logs = SPIFFS.open(LOGS_LOCATION, "r");
  
    uint8_t* log_data = (uint8_t *)ps_calloc(logs.size() + 1, sizeof(char));
    int i = 0;
    for (i = 0; i < logs.size(); i++) {
      log_data[i] = logs.read();
    }
    log_data[i] = '\0';
      
    logs.close();


    
    
    int httpResponseCode = http.POST((char*)log_data);
      
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
      
    // End HTTP Connection

    M5.Lcd.print("SENT");
    M5.Lcd.print(httpResponseCode);
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
}











/**
 * Custom version of delay which waits for GPS data to be read from serial input
 */
static void smartDelay(unsigned long ms) {
  unsigned long start = millis();
  do {
    while (ss.available()) gps.encode(ss.read());
  } while (millis() - start < ms);
}
