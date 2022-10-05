// HTTP POST Request
#include <WiFi.h>
#include <HTTPClient.h>

// Location where GPS Logs will be stored for a single hike
String LOGS_LOCATION = "/logs.txt";

// Location where Audio Files will be stored for a single hike
const char* AUDIO_LOCATIONS[10] = {"/recording.wav", "/recording1.wav", "/recording2.wav", "/recording3.wav", "/recording4.wav",
      "/recording5.wav", "/recording6.wav", "/recording7.wav", "/recording8.wav", "/recording9.wav"};

volatile int recording_index = 0;

float audio_latitudes[10];

float audio_longitudes[10];
// Flag to check that multiple attempts to connect to the Internet are not
// attempted to prevent duplication
volatile bool wifi_connection_attempted = false;

/**
 * Attempts to send a POST request through HTTP. If an internet connection has not yet
 * been established, it will keep attempting until it finds one. 
 * 
 * The POST request will send data for GPS Logs and the WAV sound files.
 */
void gps_HTTP_request() {
  
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
    bool passing = false;
    do {
      HTTPClient http;
    
      // Your Domain name with URL path or IP address with path
      http.begin(gps_server);
  
      //If you need an HTTP request with a content type: text/plain
      http.addHeader("Content-Type", "text/plain");
  
      http.addHeader("timestamp", timestamp);

      http.addHeader("endtimestamp", end_timestamp);
  
      http.addHeader("userid", "test");
  
      logs = LittleFS.open(LOGS_LOCATION, FILE_READ);
    
      uint8_t* log_data = (uint8_t *)ps_calloc(logs.size() + 1, sizeof(char));
      int i = 0;
      for (i = 0; i < logs.size(); i++) {
        log_data[i] = logs.read();
      }
      log_data[i] = '\0';
        
      logs.close();
  
      Serial.println("Publishing Message");
      
      int httpResponseCode = http.POST((char*)log_data);
        
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);

      passing = httpResponseCode == 200;
        
      // End HTTP Connection
      upload_UI(passing);
      
      http.end();

      delay(2000);
    } while (!passing);
  } else {
    Serial.println("WiFi Disconnected");
  }
}

void audio_HTTP_request() {

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
    for (int i = 0; i < recording_index; i++) {
      publish_audio(i);
      join_audio_data(i);
    }
  } else {
    Serial.println("WiFi Disconnected");
  }
}

void send_audio_segment(char* audio_data, int audio_index, int segment_index) {
  bool passing = false;  
  
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WIFI NOT CONNECTED");
  }
  
  do {
    HTTPClient http;
  
    http.begin(audio_server);
  
    http.addHeader("userid", "test");

    http.addHeader("timestamp", timestamp);

    http.addHeader("Content-Type", "text/plain");

    char index_buffer[10];
    sprintf(index_buffer, "%d", audio_index);
    http.addHeader("audioindex", index_buffer);

    sprintf(index_buffer, "%d", segment_index);
    http.addHeader("segmentindex", index_buffer);

    Serial.println("Publishing Message");
    
    int httpResponseCode = http.POST((char*)audio_data);
    
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    Serial.print("HTTP Response Message: ");
    Serial.println(http.getString());

    passing = httpResponseCode == 200;
    
    // End HTTP Connection
    upload_UI(passing);
    
    http.end(); 
  } while (false);//!passing);
}

void publish_audio(int audio_index) {

  int segment_size = 16000;

  audio_file = LittleFS.open(AUDIO_LOCATIONS[audio_index], FILE_READ);

  uint8_t* audio_buffer = (uint8_t*)ps_calloc(audio_file.size() + 1, sizeof(char));

  int i = 0;
  for (i = 0; i < audio_file.size(); i++) {
    audio_buffer[i] = audio_file.read();
  }
  audio_buffer[i] = '\0';

  int encoded_buffer_length = audio_file.size() * 2;
  uint8_t* encoded_audio_buffer = (uint8_t*)ps_calloc(encoded_buffer_length, sizeof(char));

  size_t length;

  int error = mbedtls_base64_encode(encoded_audio_buffer, encoded_buffer_length, &length,
                                    (const byte*)audio_buffer, audio_file.size() + 1);

  audio_file.close();
  free(audio_buffer);

  Serial.println(length);

  i = 0;
  int segment_index = 0;
  while (i + segment_size < length) {
    Serial.print("Starting Segment ");
    Serial.println(segment_index);
    uint8_t* audio_data = (uint8_t*)ps_calloc(segment_size + 100, sizeof(char));
    int j = 0;
    for (j = 0; j < segment_size; j++) {
      audio_data[j] = encoded_audio_buffer[i + j];
    }
    audio_data[j] = '\0';
    
    send_audio_segment((char*)audio_data, audio_index, segment_index);

    free(audio_data);

    i += segment_size;
    segment_index++;
    Serial.println("Finishing Segment");
    delay(2000);
  }

  Serial.println("Starting Last Message");

  uint8_t* audio_data = (uint8_t *)ps_calloc(segment_size + 100, sizeof(char));

  int j = 0;
  for (j = 0; j < length - i; j++) {
    audio_data[j] = encoded_audio_buffer[i + j];
  }
  audio_data[j] = '\0';

  send_audio_segment((char*)audio_data, audio_index, segment_index);

  Serial.println("Finishing Last Message");

  free(audio_data);
  free(encoded_audio_buffer);
}

void join_audio_data(int audio_index) {
  bool passing = false;
  do {
    HTTPClient http;
  
    http.begin(audio_join_server);
  
    http.addHeader("userid", "test");

    http.addHeader("timestamp", timestamp);

    http.addHeader("Content-Type", "text/plain");
    
    char index_buffer[10];
    sprintf(index_buffer, "%d", audio_index);
    http.addHeader("audioindex", index_buffer);

    char latitude_buffer[20];
    sprintf(latitude_buffer, "%f", audio_latitudes[audio_index]);
    http.addHeader("latitude", latitude_buffer);

    char longitude_buffer[20];
    sprintf(longitude_buffer, "%f", audio_longitudes[audio_index]);
    http.addHeader("longitude", longitude_buffer);

    Serial.println("Publishing Message");
  
    int httpResponseCode = http.GET();
          
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);

    passing = httpResponseCode == 200;
    
    // End HTTP Connection
    upload_UI(passing);
    
    http.end(); 
  } while (!passing);
}
