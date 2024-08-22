#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Define the reed sensor pin
const int reedSensorPin = 16; // GPIO 16

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
  display.clearDisplay(); // clear display
  display.setCursor((SCREEN_WIDTH - width) / 2, (SCREEN_HEIGHT - height) / 2);
  display.println(toPrint); // text to display
}

void setup() {
  // Initialize serial communication
  Serial.begin(115200);

  // Initialize the OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  //display.display();
  delay(2000);  // Pause for 2 seconds
  // Clear the display
  display.clearDisplay();  
  display.setTextColor(SSD1306_WHITE);  
  display.setCursor(0,0);
  // Set the reed sensor pin as input
  pinMode(reedSensorPin, INPUT_PULLUP);
}

void loop() {
  // Read the state of the reed sensor
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
    display.setCursor(0,0);
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
    display.setCursor(25,15);
    display.printf(F("LAST SHOT: %3d\n"), elapsedTime);
  }

  display.display();

  // Small delay to avoid flickering and reduce CPU load
  delay(100);
}