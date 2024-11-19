#include <Arduino.h>               // Include the core Arduino library
#include <Adafruit_SSD1306.h>      // Include the Adafruit library for the SSD1306 OLED display
#include "example_config.h"        // Include the configuration header file (presumably contains constants and settings)
#include "SmartCube/cubeSound.h"   // Include custom header for sound functions
#include "SmartCube/cubeButtons.h" // Include custom header for button handling functions
#include "SmartCube/cubeWifiManager.h" // Include custom header for managing WiFi connections

// Initialize the OLED display with specified width, height, and reset pin
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Initialize the WiFi manager, passing the display object for any display output
cubeWifiManager cubeWifiManager(display);

void setup() {
  cubeWifiManager.start(); // Start the WiFi manager
}

void loop() {
  cubeButtonHandler(); // Continuously check and handle button actions
}
