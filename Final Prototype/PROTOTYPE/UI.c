void initiate_display() {
  M5.Lcd.fillScreen(BLACK);

  M5.Lcd.setCursor(95, 10);
  M5.Lcd.setTextSize(1);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.printf("TIMESTAMP: ");
  M5.Lcd.setTextColor(RED);
  M5.Lcd.printf("UNAVAILABLE");

  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(125, 30);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.printf("RECORD");
  M5.Lcd.fillCircle(160, 135, 75, 0x7bef);

  enable_gps_UI();

  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(UPLOAD_X, UPLOAD_Y); 
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.printf("UPLOAD");
}

/**
 * Updates the UI of the GPS label
 */
void enable_gps_UI() {
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(GPS_X, GPS_Y);
  if (gps_enabled) {
    M5.Lcd.setTextColor(GREEN);
  } else {
    M5.Lcd.setTextColor(RED);
  }
  M5.Lcd.printf("GPS"); 
}

/**
 * Updates the UI of the Upload label depending on
 * the status of the HTTP message
 */
void upload_UI(bool successful) {
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(UPLOAD_X, UPLOAD_Y);
  if (successful) {
    M5.Lcd.setTextColor(GREEN);
  } else {
    M5.Lcd.setTextColor(RED);
  }
  M5.Lcd.printf("UPLOAD");
}

/**
 * Updates the UI of the Upload label depending on
 * the status of the HTTP message
 */
void timestamp_UI(char* timestamp) {

  M5.Lcd.setCursor(95, 10);
  M5.Lcd.setTextSize(1);
  M5.Lcd.setTextColor(BLACK);
  M5.Lcd.printf("TIMESTAMP: ");
  M5.Lcd.setTextColor(BLACK);
  M5.Lcd.printf("UNAVAILABLE");

  M5.Lcd.setCursor(75, 10);
  M5.Lcd.setTextSize(1);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.printf("TIMESTAMP: ");
  M5.Lcd.setTextColor(GREEN);
  M5.Lcd.printf(timestamp);
}
