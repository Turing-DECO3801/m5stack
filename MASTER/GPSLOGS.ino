// M5 Stack CORE2
#include <M5Core2.h>

// HTTP POST Request
#include <WiFi.h>
#include <HTTPClient.h>

// GPS Library
#include <TinyGPSPlus.h>

// File Storage Library
#include <LittleFS.h>

// Audio Recording Library and Definitions

#include <driver/i2s.h>

#define I2S_WS 0
#define I2S_SD 34
#define I2S_SCK 12
#define I2S_PORT I2S_NUM_0
#define I2S_SAMPLE_RATE   (16000)
#define I2S_SAMPLE_BITS   (16)
#define I2S_READ_LEN      (16 * 1024)
#define RECORD_TIME       (20) //Seconds
#define I2S_CHANNEL_NUM   (1)
#define FLASH_RECORD_SIZE (I2S_CHANNEL_NUM * I2S_SAMPLE_RATE * I2S_SAMPLE_BITS / 8 * RECORD_TIME)

// Size of the WAV file header
const int headerSize = 44;



// WIFI Credentials **SHOULD BE CHANGED TO WHATEVER THE USER'S DEFAULT SETTING IS**
const char* ssid = "Elevate-41Warren";
const char* password = "Dyingfromexams";

// Server that will receive the HTTP POST request
String serverName = "https://2i5agd6rwacftajs3skgtcrlne0vqcgc.lambda-url.ap-southeast-2.on.aws/";

// Location where GPS Logs will be stored for a single hike
String LOGS_LOCATION = "/logs.txt";

// Location where Audio Files will be stored for a single hike
String AUDIO_LOCATION = "/recording.wav";

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

  // LittleFS Setup
  format_LittleFS();
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
      publish_HTTP_request();
    }
    button_B_pressed = true;
    last_B_time = millis();
  }



  if (last_A_time + 200 < millis()) {
    button_A_pressed = false;
  }
  
  if (last_B_time + 200 < millis()) {
    button_B_pressed = false;
  }

  if (last_B_time + 200 < millis() && last_A_time + 200 < millis()) {
    M5.Axp.SetLDOEnable(3, false);
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
  smart_delay(0);
  if (gps.location.isValid()) {
    smart_delay(0);
    TinyGPSDate d = gps.date;
    TinyGPSTime t = gps.time;
    
    smart_delay(0);
    lat = gps.location.lat();
    
    smart_delay(0);
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
  logs = LittleFS.open(LOGS_LOCATION, "a");

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












// Global variable of the Object used for Reading and Writing to the Audio file
File audioFile;

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
  LittleFS.remove(AUDIO_LOCATION);
  audioFile = LittleFS.open(AUDIO_LOCATION, FILE_WRITE);
  if(!audioFile){
    M5.Lcd.println("CLEARING EXISTING AUDIO FILES");
  }

  byte header[headerSize];
  wav_header(header, FLASH_RECORD_SIZE);

  audioFile.write(header, headerSize);

  // Create the file to be appended to
  logs = LittleFS.open(LOGS_LOCATION, FILE_WRITE);
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
void publish_HTTP_request() {

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


    

    logs = LittleFS.open(LOGS_LOCATION, "r");
  
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
 * Initialises the I2S Audio module based on the configurations that are
 * suitable for the microphone
 */
void i2s_init(){

  i2s_driver_uninstall(I2S_PORT);
  
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX | I2S_MODE_PDM),
    .sample_rate = I2S_SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_RIGHT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 64,
    .dma_buf_len = 1024,
    .use_apll = 1
  };

  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);

  const i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SCK,
    .ws_io_num = I2S_WS,
    .data_out_num = -1,
    .data_in_num = I2S_SD
  };

  i2s_set_pin(I2S_PORT, &pin_config);

  i2s_set_clk(I2S_PORT, I2S_SAMPLE_RATE, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_MONO);
}

/**
 * Scales the volume of the recorded data
 */
void i2s_adc_data_scale(uint8_t * d_buff, uint8_t* s_buff, uint32_t len)
{
    uint32_t j = 0;
    uint32_t dac_value = 0;
    for (int i = 0; i < len; i += 2) {
        dac_value = ((((uint16_t) (s_buff[i + 1] & 0xf) << 8) | ((s_buff[i + 0]))));
        d_buff[j++] = 0;
        d_buff[j++] = dac_value * 256 / 2048;
    }
}

/**
 * Function that will used in another thread, this will allow for the
 * reading and storage of the data
 */
void i2s_adc(void *arg)
{
    
    int i2s_read_len = I2S_READ_LEN;
    int flash_wr_size = 0;
    size_t bytes_read;

    char* i2s_read_buff = (char*) calloc(i2s_read_len, sizeof(char));
    uint8_t* flash_write_buff = (uint8_t*) calloc(i2s_read_len, sizeof(char));

    i2s_read(I2S_PORT, (void*) i2s_read_buff, i2s_read_len, &bytes_read, portMAX_DELAY);
    
    Serial.println(" *** Recording Start *** ");
    while (flash_wr_size < FLASH_RECORD_SIZE) {
        //read data from I2S bus, in this case, from ADC.
        i2s_read(I2S_PORT, (void*) i2s_read_buff, i2s_read_len, &bytes_read, portMAX_DELAY);
        //example_disp_buf((uint8_t*) i2s_read_buff, 64);
        //save original data from I2S(ADC) into flash.
        i2s_adc_data_scale(flash_write_buff, (uint8_t*)i2s_read_buff, i2s_read_len);
        audioFile.write((const byte*) i2s_read_buff, i2s_read_len);
        flash_wr_size += i2s_read_len;
    }
    audioFile.close();

    free(i2s_read_buff);
    i2s_read_buff = NULL;
    free(flash_write_buff);
    flash_write_buff = NULL;
    
    vTaskDelete(NULL);
}


/**
 * Fill the memory with the corresponding WAV file header.
 */
void wav_header(byte* header, int wavSize){
  header[0] = 'R';
  header[1] = 'I';
  header[2] = 'F';
  header[3] = 'F';
  unsigned int fileSize = wavSize + headerSize - 8;
  header[4] = (byte)(fileSize & 0xFF);
  header[5] = (byte)((fileSize >> 8) & 0xFF);
  header[6] = (byte)((fileSize >> 16) & 0xFF);
  header[7] = (byte)((fileSize >> 24) & 0xFF);
  header[8] = 'W';
  header[9] = 'A';
  header[10] = 'V';
  header[11] = 'E';
  header[12] = 'f';
  header[13] = 'm';
  header[14] = 't';
  header[15] = ' ';
  header[16] = 0x10;
  header[17] = 0x00;
  header[18] = 0x00;
  header[19] = 0x00;
  header[20] = 0x01;
  header[21] = 0x00;
  header[22] = 0x01;
  header[23] = 0x00;
  header[24] = 0x80;
  header[25] = 0x3E;
  header[26] = 0x00;
  header[27] = 0x00;
  header[28] = 0x00;
  header[29] = 0x7D;
  header[30] = 0x00;
  header[31] = 0x00;
  header[32] = 0x02;
  header[33] = 0x00;
  header[34] = 0x10;
  header[35] = 0x00;
  header[36] = 'd';
  header[37] = 'a';
  header[38] = 't';
  header[39] = 'a';
  header[40] = (byte)(wavSize & 0xFF);
  header[41] = (byte)((wavSize >> 8) & 0xFF);
  header[42] = (byte)((wavSize >> 16) & 0xFF);
  header[43] = (byte)((wavSize >> 24) & 0xFF);

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