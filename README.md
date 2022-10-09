# M5Stack CORE2 Hardware Device

## How To Use

To use the provided code, a combined version of all the code can be found in the DevelopmentCode/MASTER directory as well as other programs that were created to assist in the development of the final file.

After obtaining the device, the first thing required is to download the Arduino IDE, the ESPTOOL Drivers and any required libraries as found at the bottom of the page within the Arduino Library and Program manager.

[CORE2 Installation Guide](https://docs.m5stack.com/en/quick_start/core2_for_aws/arduino)

Before compilation and programming of the device, settings on the device such as the servers that the device will attempt to send HTTP requests to and the WIFI connection must be made prior to the compilation of the device.

```
String gps_server = "________________________________";

String audio_server = "________________________________";

String audio_join_server = "________________________________";
```

Connection of the WIFI must also be established, providing the SSID and Password for the connection

```
const char* ssid = "________________________________";

const char* password = "________________________________";
```

After installation, the program will need to be compiled and programmed onto the ESP32 device through a USB3 to USBC cable.

## Device

<p align="center" width="100%">
  <img style="width:35%;" src="https://user-images.githubusercontent.com/86467852/194073329-e4f087b5-56d3-4f1d-bb0a-2bd5108f0e92.png" />
</p>
The hardware device that has been used as the prototype for data recording was the M5STACK CORE2 due to its small form factor and built-in functions such as the microphone and easily attachable sensors like the GPS sensor. The device uses an ESP32 microcontroller that is capable of WIFI connections and is compatible with numerous libraries that were required for the overall product.

## Functionality

![image](https://user-images.githubusercontent.com/86467852/194072697-dbefc976-19df-4449-858a-f4254b5fdb78.png)

### UI
The User Interface of the Hardware device has been made to imitate that of the ideal version of the device. The UI of the device has been kept simple to allow users to handle and interact with the device with only a single hand and without having to fiddle or adjust it. Some UI features are the 3 buttons, a large audio recording to be done without being seen and 2 smaller ones to start GPS logging and to upload. The UI also has visual and haptic feedback for button presses and to signal when audio or GPS data is being recorded.

### GPS Logging
As of the final prototype, the program is capable of starting and stopping the recording of GPS data that will automatically log it into a file for later retrieval in upload. The device will intermittently poll the GPS sensor for location data and append it to the end of a log. This data will be used to track the location of audio memos and the path of the hike for them to review as a visualised path on the front end application.

On the device, to start logging GPS data, the GPS button can be pressed with the colour green indicating that GPS is currently being tracked and red indicating that is currently turned off.

### Audio Recording
Audio recording 

The device is capable of recording audio by pressing the the large button in the centre of the touchscreen display. A vibration will occur on initial touch to start recording as well as final touch to end the recording.

### Data Upload

The upload of data with the device requires a stable internet connection which the user will do once they have finished the hike and have reached home. This upload of data will interact with Severless Functions to store the data in their relevant databases and large file storage. These databases will also be used to retrieve data on the front end.

## Libraries

### M5CORE2 Library

[M5CORE2](https://github.com/m5stack/M5Core2)

This library was the default library that was provided by M5STACK to program the UI of the device. This library allowed for haptic feedback in the form of vibrations, audio feedback and input through the touch screen display and extra buttons. This library also provided base code to other features such as the file system LittleFS and GPS example code that was adapted to the use seen in the current program.

### WIFI, HTTPClient and BASE64/mbedtls

[HttpClient Documentation](https://github.com/amcewen/HttpClient/)

The WIFI and HTTPClient were used for connection to the AWS Lambda functions to upload GPS logs and audio data. The BASE64 library was used when uploading the data to ensure that there was no loss or errors in the raw data that was being transferred so it was first converted to this encoding and would be later decoded on the Severless Functions.

### I2S

[I2S Microphone](https://github.com/espressif/esp-idf/tree/master/examples/peripherals/i2s)

[I2S Recording Example Code](https://github.com/0015/ThatProject/blob/master/ESP32_MICROPHONE/ESP32_INMP441_RECORDING/ESP32_INMP441_RECORDING.ino)

The I2S library allows the device to convert raw audio data from the internal microphone and convert it and store it as a WAV file. The documentation and code provided 

### TinyGPSPlus

The TinyGPSPlus library was used with the external GPS sensor that was attached through the PORT A pins of the M5STACK CORE2. This library allowed for different conversion of data into date time and latitude and longitude values that are compatible with the front end pathing APIs.

### LittleFS

LittleFS was the file system that was used to store GPS logs and Audio recordings. An alternate system was used before called SPIFFS but this file system was far too slow and would often cause data logged to be corrupted or missing.

