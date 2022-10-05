// M5 Stack CORE2
#include <M5Core2.h>

#include SECRETS.h
#include AUDIO.c
#include FILESYS.c
#include UI.h 
#include AUDIO.h
#include UPLOAD.c

// The serial connection to the GPS device
HardwareSerial ss(2);

void setup() {
  M5.begin();

  // Serial Debugging Setup
  Serial.begin(115200); 

  // GPS Setup
  ss.begin(GPSBaud, SERIAL_8N1, 33, 32);

  WiFi.mode(WIFI_STA);


  // LittleFS Setup
  format_LittleFS();

  // Audio Recording Setup
  i2s_init();

  // UI Setup
  initiate_display();
}


bool button_A_pressed = false;

bool button_C_pressed = false;

bool button_rec_pressed = false;

long int last_A_time = millis();

long int last_C_time = millis();

long int last_rec_time = millis();

long int last_log_time = millis();

volatile boolean gps_enabled = false;

volatile boolean recording_enabled = false;

void loop() {
  M5.update();

  check_button_A();
  check_button_C();
  check_button_record();


  if (last_A_time + 200 < millis()) {
    button_A_pressed = false;
  }
  
  if (last_C_time + 200 < millis()) {
    button_C_pressed = false;
  }

  if (last_rec_time + 200 < millis()) {
    button_rec_pressed = false;
  }

  if (last_log_time + 10000 < millis()) {
    log_GPS_data();
    last_log_time = millis();
  }

  if (!button_C_pressed && !button_A_pressed && !button_rec_pressed) {
    M5.Axp.SetLDOEnable(3, false);
  }
}

/**
 * Enables the logging of GPS data into storage on the device
 * and updates the UI accordingly.
 */
void check_button_A() {
  if (M5.BtnA.wasReleased() || M5.BtnA.pressedFor(0, 200)) {
    M5.Axp.SetLDOEnable(3, true);
    if (!button_A_pressed) {
      gps_enabled = !gps_enabled;
      enable_gps_UI();
    }
    button_A_pressed = true;
    last_A_time = millis();
  }
}

/**
 * Checks if a press on the main play/pause button has occurred and
 * carries out the required functionality to handle a recording.
 */
void check_button_record() {
  TouchPoint_t pos = M5.Touch.getPressPoint();
  if (pos.x != -1) {
    if (pos.x > 100 && pos.x < 220 && pos.y > 95 && pos.y < 195) {
      M5.Axp.SetLDOEnable(3, true);
      if (!button_rec_pressed) {
        if (!recording_enabled) {
          M5.Lcd.fillCircle(160, 135, 75, GREEN);

          prepare_audio_recording();
          
          xTaskCreate(i2s_adc, "i2s_adc", 1024 * 2, NULL, 1, NULL);
        } else {
          M5.Lcd.fillCircle(160, 135, 75, 0x7bef);
        }
        recording_enabled = !recording_enabled;
      }
      button_rec_pressed = true;
      last_rec_time = millis();
    }
  }
}

/**
 * Helper function to set the correct Audio File pointer and append
 * data for the GPS location of where the recording was made.
 */
void prepare_audio_recording() {
  audio_file = LittleFS.open(AUDIO_LOCATIONS[recording_index], FILE_WRITE);
  if(!audio_file){
    Serial.println("File is not available!");
  }

  if (gps.location.isValid()) {
    smart_delay(0);
    audio_latitudes[recording_index] = gps.location.lat();
  
    smart_delay(0);
    audio_longitudes[recording_index] = gps.location.lng();
  }

  byte header[header_size];
  wav_header(header, FLASH_RECORD_SIZE);

  audio_file.write(header, header_size);
}

/**
 * Checks to see if button C was recently pressed to prevent repeated
 * functionality on a single press.
 */
void check_button_C() {
  if (M5.BtnC.wasReleased() || M5.BtnC.pressedFor(0, 200)) {
    M5.Axp.SetLDOEnable(3, true);
    if (!button_C_pressed) {
      gps_HTTP_request();
      audio_HTTP_request();
    }
    button_C_pressed = true;
    last_C_time = millis();
  }
}
