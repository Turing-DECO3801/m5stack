// GPS Library
#include <TinyGPSPlus.h>

// GPS Baud Rate
static const uint32_t GPSBaud = 9600;

//TinyGPS++ Global Object for retrieval of GPS data
TinyGPSPlus gps;

// Determines if a unique timestamp has been added
bool timestamp_added = false;

// Location for the storage of the timestamp
char timestamp[48];

char end_timestamp[48];

// Latitude used for logs
float lat;

// Longitude used for logs
float lng;

// Current Time used for logs
char tme[16];

// Current Date used for logs
char dte[16];

/**
 * Stores the GPS data in a text file for later retrieval and posting to AWS Lambda
 */
void log_GPS_data() {
  smart_delay(0);
  if (gps.location.isValid() && gps_enabled) {
    smart_delay(0);
    TinyGPSDate d = gps.date;
    TinyGPSTime t = gps.time;
    
    smart_delay(0);
    lat = gps.location.lat();
    
    smart_delay(0);
    lng = gps.location.lng();
  
    
    sprintf(dte, "%02d-%02d-%02d", d.year(), d.day(), d.month());
    sprintf(tme, "%02d:%02d:%02d", t.hour(), t.minute(), t.second());
    sprintf(end_timestamp, "%02d-%02d-%02d %02d:%02d:%02d", d.year(), d.month(), d.day(), t.hour(), t.minute(), t.second());
    if (!timestamp_added) {
      sprintf(timestamp, "%02d-%02d-%02d %02d:%02d:%02d", d.year(), d.month(), d.day(), t.hour(), t.minute(), t.second());
      timestamp_added = true;
      timestamp_UI(timestamp);
    }
      
    append_to_logs();
  }
}

/**
 * Adds the updated GPS values to the file at location LOGS_LOCATION
 */
void append_to_logs() {
  logs = LittleFS.open(LOGS_LOCATION, FILE_APPEND);

  char lineBuffer[30];
  sprintf(lineBuffer, "%f,%f,", lat, lng);
  logs.print(lineBuffer);
  logs.print(dte);
  logs.print(",");
  logs.print(tme);
  logs.print("\r\n");

  logs.close();
}

/**
 * Test function to print out GPS log data to the M5Core2 Display
 */
void print_log_data() {
  logs = LittleFS.open(LOGS_LOCATION, "r");

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
 * Custom version of delay which waits for GPS data to be read from serial input
 */
static void smart_delay(unsigned long ms) {
  unsigned long start = millis();
  do {
    while (ss.available()) gps.encode(ss.read());
  } while (millis() - start < ms);
}
