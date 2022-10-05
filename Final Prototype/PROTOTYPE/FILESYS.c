// File Storage Library
#include <LittleFS.h>

// Global variable of the Object used for Reading and Writing to the Audio file
File audio_file;

// Global variable of the Object used for Reading and Writing to the Log file
File logs;

/**
 * Formats the LittleFS if not done so already, then it will attempt to open a new file
 */
void format_LittleFS() {
  M5.Lcd.println("LittleFS format start...");
//  LittleFS.format();  // Formatting LittleFS
  M5.Lcd.println("LittleFS format finish");

  LittleFS.begin(true);

  // Clears the file to ensure that all logs are fresh
  if (LittleFS.exists(LOGS_LOCATION)) {
    M5.Lcd.println("CLEARING EXISTING LOG FILES");
    LittleFS.remove(LOGS_LOCATION);
  }

  // Clears the file to ensure that the audio files are fresh
  for (int i = 0; i < 10; i++) {
    LittleFS.remove(AUDIO_LOCATIONS[i]);  
  }

  // Create the file to be appended to
  logs = LittleFS.open(LOGS_LOCATION, FILE_WRITE);
  M5.Lcd.println("CREATING NEW LOG FILES");
  logs.close();
}
