Display UK train departures using ESP32 + ePaper Display

Here's how to build a train departure board using an ESP32, a Waveshare e-Paper display panel an API call to retrieve some Json data. In this particular example I've used it to display a list of departures from a specific UK train station. You could easily modify it to display departure data from another rail hub or even an airport. If you're using the display on a model railway or diorama you could of course just generate your own data without using a 3rd party data provider.

Full code walkthrough in this video:

[![Live UK Train Departures Board (ESP32 + ePaper Display)](https://img.youtube.com/vi/GDFLaLhaBBA/0.jpg)](https://www.youtube.com/watch?v=GDFLaLhaBBA)

Click on the image above to watch the video or go here: https://youtu.be/GDFLaLhaBBA

Check out my YouTube channel for other tutorials on how to use the ESP32 and other microcontrollers to display data on ePaper and OLED displays.

My Arduino IDE source code: https://github.com/nichelaboratory/GxEPD2_Train_Departures_Board
UK transport data source as used in video (has a free plan): https://www.transportapi.com/
Transport API's github repository: https://github.com/transportapi
National Rail have railway data API's: https://www.nationalrail.co.uk/developers/
Nice list of free transportation public APIs: https://github.com/public-apis/public-apis#transportation
Tool for formatting JSON into a more readable format: https://jsonformatter.org/json-pretty-print

In this video I'm using a regular ESP32 and the Waveshare 280x480 pixel monochrome e-ink display. You could use any other panel or even replace it with a TFT or OLED display. Just be aware that screen burn can be an issue with departure board displays.

Arduino IDE libraries used in this example: WiFiClient and WiFiClientSecure, ArduinoJson and GxEPD2.