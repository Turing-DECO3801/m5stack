# M5STACK CORE2 GPS Tracking, Voice Recording and Upload to AWS



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

This library was the default library that was provided by M5STACK to program the UI of the device. This library allowed for haptic feedback in the form of vibrations, audio feedback and input through the touch screen display and extra buttons.

### WIFI, HTTPClient and BASE64/mbedtls

The WIFI and HTTPClient were used for connection to the AWS Lambda functions to upload GPS logs and audio data. The BASE64 library was used when uploading the data to ensure that there was no loss or errors in the raw data that was being transferred so it was first converted to this encoding and would be later decoded on the Severless Functions.

### I2S 

The I2S library allows the device to convert raw audio data from the internal microphone and convert it and store it as a WAV file.

### TinyGPSPlus

The TinyGPSPlus library was used with the external GPS sensor that was attached through the PORT A pins of the M5STACK CORE2. This library allowed for different conversion of data into date time and latitude and longitude values that are compatible with the front end pathing APIs.

### LittleFS

LittleFS was the file system that was used to store GPS logs and Audio recordings.

