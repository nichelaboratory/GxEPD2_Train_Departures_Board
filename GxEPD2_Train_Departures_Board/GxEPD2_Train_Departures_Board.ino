//Retrieve data for a specific UK train station's departures and display on a Waveshare ePaper display
//See this video for a code walkthrough and demonstration of finished version: TBC
//
//EPAPER DISPLAY README:
//
//Example is based on the GxEPD2_HelloWorld.ino example in the GxEPD2 library by Jean-Marc Zingg
//This code assumes you're using the Waveshare 3.7 inch e-Paper HAT (480x280 pixels)
//If you're using another display then you'll have to comment out the 3.7 inch and
//uncomment your display in GxEPD2_display_selection_new_style
//You will also need to alter the co-ordinates of what's rendered on the screen
//This code is intended for the ESP32 microcontroller but could be adapted for other
//devices running Arduino C.
//
//DATA SOURCE README:
//
//You can get "free" UK train data using the API at https://www.transportapi.com/
//On a free plan there is a data limit. Also note their API is somewhat confusing
//In this demo I'm using an example of their JSON reuploaded to here:
//https://raw.githubusercontent.com/nichelaboratory/GxEPD2_Train_Departures_Board/main/sample_data.json
//
//SSL README:
//If you change the data source you'll need a new SSL certificate in the variable 'test_root_ca'. You might
//also need a new one if the existing certificate has expired.
//Tutorial for making SSL certificate for use with the ESP32 and other devices: https://youtu.be/dAZjWDZDFNo

#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#define MAX_DESTINATIONS_TO_DISPLAY 9 //How many lines of departures to display on the display

const char* ssid     = "YOUR_WIFI_SSID";     // your network SSID (name of wifi network)
const char* password = "YOUR_WIFI_PASSWORD"; // your network password

const char*  server = "raw.githubusercontent.com";  // Server URL
const char* stationName;
const char* stationCode;
const char* requestDate;
const char* requestTime;

// This certificate will ONLY work when connecting to raw.githubusercontent.com
// You will need to make a new certificate if you want to connect to another server via SSL!
// See tutorial here: https://youtu.be/dAZjWDZDFNo

const char* test_root_ca= \
     "-----BEGIN CERTIFICATE-----\n" \
"MIIEyDCCA7CgAwIBAgIQDPW9BitWAvR6uFAsI8zwZjANBgkqhkiG9w0BAQsFADBh\n" \
"MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n" \
"d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBH\n" \
"MjAeFw0yMTAzMzAwMDAwMDBaFw0zMTAzMjkyMzU5NTlaMFkxCzAJBgNVBAYTAlVT\n" \
"MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxMzAxBgNVBAMTKkRpZ2lDZXJ0IEdsb2Jh\n" \
"bCBHMiBUTFMgUlNBIFNIQTI1NiAyMDIwIENBMTCCASIwDQYJKoZIhvcNAQEBBQAD\n" \
"ggEPADCCAQoCggEBAMz3EGJPprtjb+2QUlbFbSd7ehJWivH0+dbn4Y+9lavyYEEV\n" \
"cNsSAPonCrVXOFt9slGTcZUOakGUWzUb+nv6u8W+JDD+Vu/E832X4xT1FE3LpxDy\n" \
"FuqrIvAxIhFhaZAmunjZlx/jfWardUSVc8is/+9dCopZQ+GssjoP80j812s3wWPc\n" \
"3kbW20X+fSP9kOhRBx5Ro1/tSUZUfyyIxfQTnJcVPAPooTncaQwywa8WV0yUR0J8\n" \
"osicfebUTVSvQpmowQTCd5zWSOTOEeAqgJnwQ3DPP3Zr0UxJqyRewg2C/Uaoq2yT\n" \
"zGJSQnWS+Jr6Xl6ysGHlHx+5fwmY6D36g39HaaECAwEAAaOCAYIwggF+MBIGA1Ud\n" \
"EwEB/wQIMAYBAf8CAQAwHQYDVR0OBBYEFHSFgMBmx9833s+9KTeqAx2+7c0XMB8G\n" \
"A1UdIwQYMBaAFE4iVCAYlebjbuYP+vq5Eu0GF485MA4GA1UdDwEB/wQEAwIBhjAd\n" \
"BgNVHSUEFjAUBggrBgEFBQcDAQYIKwYBBQUHAwIwdgYIKwYBBQUHAQEEajBoMCQG\n" \
"CCsGAQUFBzABhhhodHRwOi8vb2NzcC5kaWdpY2VydC5jb20wQAYIKwYBBQUHMAKG\n" \
"NGh0dHA6Ly9jYWNlcnRzLmRpZ2ljZXJ0LmNvbS9EaWdpQ2VydEdsb2JhbFJvb3RH\n" \
"Mi5jcnQwQgYDVR0fBDswOTA3oDWgM4YxaHR0cDovL2NybDMuZGlnaWNlcnQuY29t\n" \
"L0RpZ2lDZXJ0R2xvYmFsUm9vdEcyLmNybDA9BgNVHSAENjA0MAsGCWCGSAGG/WwC\n" \
"ATAHBgVngQwBATAIBgZngQwBAgEwCAYGZ4EMAQICMAgGBmeBDAECAzANBgkqhkiG\n" \
"9w0BAQsFAAOCAQEAkPFwyyiXaZd8dP3A+iZ7U6utzWX9upwGnIrXWkOH7U1MVl+t\n" \
"wcW1BSAuWdH/SvWgKtiwla3JLko716f2b4gp/DA/JIS7w7d7kwcsr4drdjPtAFVS\n" \
"slme5LnQ89/nD/7d+MS5EHKBCQRfz5eeLjJ1js+aWNJXMX43AYGyZm0pGrFmCW3R\n" \
"bpD0ufovARTFXFZkAdl9h6g4U5+LXUZtXMYnhIHUfoyMo5tS58aI7Dd8KvvwVVo4\n" \
"chDYABPPTHPbqjc1qCmBaZx2vN4Ye5DUys/vZwP9BFohFrH/6j/f3IL16/RZkiMN\n" \
"JCqVJUzKoZHm1Lesh3Sz8W2jmdv51b2EQJ8HmA==\n" \
"-----END CERTIFICATE-----";

WiFiClientSecure client;

//If you're downloading more than 32k then increase the buffer size here
//Take extra care if you're downloading binary content or content from a
//server that you're not in control of...
//word data type is limited to 65535 characters so change to unsigned long if you're expecting more content returned
//You might also see compilation errors if you make the buffer too large.
//A better workaround is to use the API to restrict the amount of data returned.
const word bufferSize = 32768;
char response[bufferSize];
JsonDocument doc;

#include <GxEPD2_BW.h>
//#include <GxEPD2_3C.h>
//#include <GxEPD2_4C.h>
//#include <GxEPD2_7C.h>

//Fonts are normally (on Windows) in C:\Users\YOUR_WINDOWS_USERNAME\Documents\Arduino\libraries\Adafruit_GFX_Library\Fonts
#include <Fonts/FreeMonoBold12pt7b.h>
#include <Fonts/FreeMonoBold9pt7b.h>

// select the display class and display driver class in the following file (new style):
#include "GxEPD2_display_selection_new_style.h"

// or select the display constructor line in one of the following files (old style):
//#include "GxEPD2_display_selection.h"
//#include "GxEPD2_display_selection_added.h"

#define uS_TO_S_FACTOR 1000000  //Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  3600 //3600 //Seconds 3600 seconds in one hour 3600*2 = 7200

//Class objects for characters. Add new properties as appropriate.
class Departure {
  public:
  const char* time;
  const char* destination;
  const char* platform; //Platform could be a byte but be aware some platforms are alphanumeric, e.g. platform '1A'.
  
  Departure() { }
  Departure(const char* t, const char* d, const char* p) { // Constructor with parameters
      time = t;
      destination = d;
      platform = p;
  }
};

Departure departures[MAX_DESTINATIONS_TO_DISPLAY]; //Initialise array of departures

//Instantiate the array of departures for testing offline. If you add or remove items be sure to update the array's size.
//Departure departures[9] {{"12:10", "Brighton", "7a"}, {"12:20", "London Bridge", "1"}, {"12:25", "Cambridge", "2"}, {"12:30", "Littlehampton", "2"},
//{"12:40", "Brighton", "1"}, {"12:45", "Eastbourne & Hastings", "4"}, {"12:52", "London Bridge", "3"}, {"12:55", "Bedford", "2"}
//, {"12:58", "Littlehampton", "3"}};

void setup()
{

  unsigned long serialBaudRate = 115200;

  Serial.begin(serialBaudRate);
  retrieveDepartureData();

  if (sizeof(response) / sizeof(char) > 0) {
    Serial.println("Attempting to parse Json from server...");
    parseDepartureData();
  } else {
    Serial.println("No Json retrieved from server :(");
  }

  //Initialise the Waveshare ePaper display panel
  //display.init(serialBaudRate); // default 10ms reset pulse, e.g. for bare panels with DESPI-C02
  display.init(serialBaudRate, true, 2, false); // USE THIS for Waveshare boards with "clever" reset circuit, 2ms reset pulse

  updateDisplay(); //Update the display
  display.hibernate();

}

//Updates the ePaper display panel
static void updateDisplay()
{
  display.setRotation(1);
  //display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(GxEPD_BLACK);
  display.fillScreen(GxEPD_WHITE);
  
  display.setFullWindow();
  display.firstPage();
  do
  {
    display.fillScreen(GxEPD_WHITE);

    //Header rectangle
    display.fillRect(0, 0, 480, 50, GxEPD_BLACK);

    //Heading - this might need tweaking for very long station names
    char heading[36]; 
    strcat(heading, stationName);
    strcat(heading, " Departures");

    word headingXPos = 10;
    word dataYPos = 68;

    word timeXPos = 10;
    word timeYPos = dataYPos;

    word destinationXPos = 80;
    word destinationYPos = dataYPos;

    word platformXPos = 420;
    word platformYPos = dataYPos;    

    byte verticalSpacing = 25;

    display.setCursor(headingXPos, 12);
    display.setTextColor(GxEPD_WHITE);
    display.setFont(&FreeMonoBold12pt7b);
    display.print(heading);

    display.setCursor(timeXPos, 40);
    display.setFont(&FreeMonoBold9pt7b);
    display.print("Time");

    display.setCursor(destinationXPos, 40);
    display.setFont(&FreeMonoBold9pt7b);
    display.print("Destination");

    display.setCursor(platformXPos - 40, 40);
    display.setFont(&FreeMonoBold9pt7b);
    display.print("Platform");

    display.setTextColor(GxEPD_BLACK);
    
    display.setFont(&FreeMonoBold9pt7b);

    for (int i = 0; i < (sizeof(departures) / sizeof(Departure)); ++i) {

      display.setCursor(timeXPos, timeYPos);
      display.print(departures[i].time);

      display.setCursor(destinationXPos, destinationYPos);
      display.print(departures[i].destination);

      display.setCursor(platformXPos, platformYPos);
      display.print(departures[i].platform);


      timeYPos += verticalSpacing;
      destinationYPos += verticalSpacing;
      platformYPos += verticalSpacing;

    }

  }
  while (display.nextPage());

}

void loop() {}; //Loop not required

void retrieveDepartureData() {
  Serial.print("Attempting to connect to SSID: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  // attempt to connect to Wifi network:
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    // wait 1 second for re-trying
    delay(1000);
  }

  Serial.print("Connected to ");
  Serial.println(ssid);

  client.setCACert(test_root_ca);

  Serial.println("\nStarting connection to server...");
  if (!client.connect(server, 443))
    Serial.println("Connection failed!");
  else {
    Serial.println("Connected to server!");
    //Remember to change this line so it points to whatever url you want to download from the server
    //This is retrieving https://raw.githubusercontent.com/nichelaboratory/GxEPD2_Train_Departures_Board/main/sample_data.json
    client.println("GET https://raw.githubusercontent.com/nichelaboratory/GxEPD2_Train_Departures_Board/main/sample_data.json HTTP/1.0");
    client.print("Host: ");
    client.println(server);
    client.println("Connection: close");
    client.println();

    while (client.connected()) {
      String line = client.readStringUntil('\n');
      if (line == "\r") {
        Serial.println("headers received");
        break;
      }

    }

  word bufferPosition = 0;
  bool bufferFull = false;
  unsigned long responseSize = 0;

  //If there are incoming bytes available
  //from the server, read them and put into the 'response' char array
  while (client.connected()) {

    if (client.available()) {
      char c = client.read();
      ++responseSize;
      //strncat(response, &c, 1);
      if (bufferPosition < bufferSize) {
        response[bufferPosition] = c;
        ++bufferPosition;
      } else {
        bufferFull = true;
      }
    }

  }

  client.stop();

  if (bufferFull) {
    Serial.println("Error: buffer is full!");
    Serial.print("Consider increasing buffer size. Returned content was ");
    Serial.print(responseSize);
    Serial.println(" characters long.");
    Serial.print("Buffer size is currently ");
    Serial.print(bufferSize);
    Serial.println(" characters.");
  } else {
    Serial.print("Returned content was ");
    Serial.print(responseSize);
    Serial.println(" characters long.");
    
    //Uncomment to view the raw JSON returned from the server
    //Serial.println("RESPONSE FROM SERVER:");      
    //Serial.println(response);
  }
    
  }
}

void parseDepartureData() {

  const unsigned long CAPACITY = JSON_ARRAY_SIZE(sizeof(response) / sizeof(char));

  // allocate the memory for the JSON document
  StaticJsonDocument<CAPACITY> doc;

  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, response);

  //Check if parsing succeeded
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  } else {

    // extract the values
    Serial.println("Values from JSON array:");

    stationName = doc["station_name"];
    Serial.print("Station name: ");
    Serial.println(stationName);

    stationCode = doc["station_code"];
    Serial.print("Station code: ");
    Serial.println(stationCode);

    requestDate = doc["date"];
    Serial.print("Request date: ");
    Serial.println(requestDate);

    requestTime = doc["time_of_day"];
    Serial.print("Request time: ");
    Serial.println(requestTime);

    for (int i = 0; i < doc["departures"]["all"].size(); ++i) {

      //Extract some properties from the departure. Note I'm only extracting a very limited
      //number of properties - there are many more!
      const char* trainDestination = doc["departures"]["all"][i]["destination_name"];
      const char* trainDepartureTime = doc["departures"]["all"][i]["aimed_departure_time"];
      const char* trainPlatform = doc["departures"]["all"][i]["platform"];

      //Mode should be "train". Presumably it could be "bus" for a replacement bus service or a regular
      //bus service where the rail line was closed some years ago
      if ((doc["departures"]["all"][i]["mode"] == "train") && (i < MAX_DESTINATIONS_TO_DISPLAY)) {
        
        Serial.print("Departure: ");
        Serial.print(i);
        Serial.print(" Destination: ");
        Serial.print(trainDestination);
        Serial.print(" Time: ");
        Serial.print(trainDepartureTime);
        Serial.print(" Platform: ");
        Serial.println(trainPlatform);

        departures[i] = {trainDepartureTime, trainDestination, trainPlatform};

      } else {

        Serial.print("Not displaying departure ");
        Serial.print(i);
        Serial.print(" to ");
        Serial.println(trainDestination);

      }

    }

  }

}

