
#include <M5Core2.h>

#define GPS_X 36
#define GPS_Y 220

#define UPLOAD_X 230
#define UPLOAD_Y 220

void setup() {
  M5.begin();

  Serial.begin(115200); 

  initiate_display();
}

void initiate_display() {
  M5.Lcd.fillScreen(BLACK);

  M5.Lcd.setCursor(127, 15);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(2);
  M5.Lcd.printf("RECORD");
  M5.Lcd.fillCircle(160, 120, 75, 0x7bef);

  enable_gps();

  M5.Lcd.setCursor(UPLOAD_X, UPLOAD_Y); 
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.printf("UPLOAD");
}

volatile boolean gps_enabled = true;

volatile boolean recording_enabled = false;

void enable_gps() {
  gps_enabled = !gps_enabled;
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(GPS_X, GPS_Y);
  if (gps_enabled) {
    M5.Lcd.setTextColor(GREEN);
  } else {
    M5.Lcd.setTextColor(RED);
  }
  M5.Lcd.printf("GPS"); 
}

void upload() {
  M5.Lcd.setCursor(UPLOAD_X, UPLOAD_Y);
  M5.Lcd.setTextColor(GREEN);
  M5.Lcd.printf("UPLOAD");
  uint8_t* log_data = (uint8_t *)ps_calloc(1000 * 6000 + 1, sizeof(char));

  free(log_data);
}

bool button_A_pressed = false;

bool button_C_pressed = false;

bool button_rec_pressed = false;

long int last_A_time = millis();

long int last_C_time = millis();

long int last_rec_time = millis();

void check_button_A() {
  if (M5.BtnA.wasReleased() || M5.BtnA.pressedFor(0, 200)) {
    M5.Axp.SetLDOEnable(3, true);
    if (!button_A_pressed) {
      enable_gps();
    }
    button_A_pressed = true;
    last_A_time = millis();
  }
}

void check_button_C() {
  if (M5.BtnC.wasReleased() || M5.BtnC.pressedFor(0, 200)) {
    M5.Axp.SetLDOEnable(3, true);
    if (!button_C_pressed) {
      upload();
    }
    button_C_pressed = true;
    last_C_time = millis();
  }
}

void check_button_record() {
  TouchPoint_t pos = M5.Touch.getPressPoint();
  if (pos.x != -1) {
    if (pos.x > 100 && pos.x < 220 && pos.y > 80 && pos.y < 180) {
      M5.Axp.SetLDOEnable(3, true);
      if (!button_rec_pressed) {
        recording_enabled = !recording_enabled;
      }
      button_rec_pressed = true;
      last_rec_time = millis();
    }
  }
}

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

  if (!button_A_pressed && !button_C_pressed && !button_rec_pressed) {
    M5.Axp.SetLDOEnable(3, false);
  }
  Serial.println(recording_enabled);
}
