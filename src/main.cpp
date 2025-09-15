#include "example_config.h"            // Include the configuration header file
#include <Arduino.h>                   // Include the core Arduino library
#include <Adafruit_SSD1306.h>          // Include the Adafruit library for the SSD1306 OLED display
#include "SmartCube/cubeSound.h"       // Include custom header for sound functions
#include "SmartCube/cubeButtons.h"     // Include custom header for button handling functions
#include "SmartCube/cubeWifiManager.h" // Include custom header for managing WiFi connections
#include "SmartCube/cubeDemo.h"        // Remove this if not using demo functions

// Initialize the OLED display with specified width, height, and reset pin
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Initialize the WiFi manager, passing the display object for any display output
cubeWifiManager cubeWifiManager(display);

void initSystems() {
  pinMode(PIN_BTN_L, INPUT);
  pinMode(PIN_BTN_M, INPUT);
  pinMode(PIN_BTN_R, INPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  Wire.begin();             // Initialize I2C
  Wire.setClock(400000L);    // Set I2C to Fast Mode (400 kHz)
  display.ssd1306_command(SSD1306_SETCONTRAST);
  display.ssd1306_command(200);  // Value between 0 and 255
  // Initialize the SSD1306 display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Adjust I2C address if needed
    for(;;); // Don't proceed, loop forever
  }

  // Rotate the display 180 degrees
  display.setRotation(2);

  // Clear the display buffer
  display.clearDisplay();
  display.setTextSize(1);      
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.display();
}

void setup() {
  initSystems();
  cubeWifiManager.start(); // Start the WiFi manager
}

void loop() {
  interactiveCubeDemo(); // Run the demo screen animation
  cubeButtonHandler();   // Continuously check and handle button actions
}