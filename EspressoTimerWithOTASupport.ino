#include <ArduinoOTA.h>

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <WiFi.h>
#include <ESPmDNS.h>
#include <NetworkUdp.h>
#include <ArduinoOTA.h>
#include <time.h>

const char *ssid = "...";
const char *password = "...";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 0;
const int   daylightOffset_sec = 3600;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Define the reed sensor pin
const int reedSensorPin = 16;  // GPIO 16

// Variables to track the state and time
bool isSensorTriggered = false;
unsigned long startTime = 0;
unsigned long elapsedTime = 0;
unsigned long lastTime = 0;

void oledDisplayCenter(String toPrint) {
  int16_t x1;
  int16_t y1;
  uint16_t width;
  uint16_t height;

  display.getTextBounds(toPrint, 0, 0, &x1, &y1, &width, &height);

  // display on horizontal and vertical center
  display.clearDisplay();  // clear display
  display.setCursor((SCREEN_WIDTH - width) / 2, (SCREEN_HEIGHT - height) / 2);
  display.println(toPrint);  // text to display
}

void setup() {
  // Initialize serial communication
  Serial.begin(115200);

  // Initialize the OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }
  //display.display();
  delay(2000);  // Pause for 2 seconds
  // Clear the display
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);

  Serial.println("Booting");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  printLocalTime();

  // Port defaults to 3232
  // ArduinoOTA.setPort(3232);

  // Hostname defaults to esp3232-[MAC]
  // ArduinoOTA.setHostname("myesp32");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH) {
        type = "sketch";
      } else {  // U_SPIFFS
        type = "filesystem";
      }

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) {
        Serial.println("Auth Failed");
      } else if (error == OTA_BEGIN_ERROR) {
        Serial.println("Begin Failed");
      } else if (error == OTA_CONNECT_ERROR) {
        Serial.println("Connect Failed");
      } else if (error == OTA_RECEIVE_ERROR) {
        Serial.println("Receive Failed");
      } else if (error == OTA_END_ERROR) {
        Serial.println("End Failed");
      }
    });

  ArduinoOTA.begin();

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Initialize the OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;
  }
  //display.display();
  delay(2000);  // Pause for 2 seconds
  // Clear the display
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("IP address: ");
  display.println(WiFi.localIP());
  display.display();
  delay(10000);
  // Set the reed sensor pin as input
  pinMode(reedSensorPin, INPUT_PULLUP);
}

void loop() {
  // Read the state of the reed sensor

  ArduinoOTA.handle();

  bool currentSensorState = digitalRead(reedSensorPin) == LOW;

  if (currentSensorState) {
    // Sensor is triggered (LOW)
    if (!isSensorTriggered) {
      // Sensor was just triggered
      isSensorTriggered = true;
      startTime = millis();

    }
    // Calculate elapsed time
    elapsedTime = (millis() - startTime) / 1000;  // Convert milliseconds to seconds

    // Update the OLED display with the elapsed time
    display.clearDisplay();
    display.setTextSize(4);
    display.setCursor(0, 0);
    String str = String(elapsedTime);
    oledDisplayCenter(str);
  } else {
    // Sensor is not triggered (HIGH)
    if (isSensorTriggered) {
      // Sensor was just untriggered
      isSensorTriggered = false;
    }

    // Clear the display or show a message
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(25, 10);
    display.printf(F("LAST SHOT: %3d\n"), elapsedTime);
    display.setCursor(25, 20);
    printLocalTime();

  }

  display.display();

  // Small delay to avoid flickering and reduce CPU load
  delay(100);
}

void printLocalTime(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  display.println(&timeinfo, "%H:%M:%S");
  
}
