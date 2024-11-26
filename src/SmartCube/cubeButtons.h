extern Adafruit_SSD1306 display;    // Reference to the OLED display object
bool is_display_off = false;        // State variable to track display on/off status

unsigned long buttonDebounceDelay = 50;       // Debounce delay in milliseconds
unsigned long buttonLongPressDelay = 2000;    // Long press threshold in milliseconds

void cubeButtonHandler() {
    unsigned long currentMillis = millis(); // Get the current time in milliseconds

    // Track the start times of button presses
    static unsigned long leftPressStart = 0;
    static unsigned long middlePressStart = 0;
    static unsigned long rightPressStart = 0;

    // Track if a beep has already been played for each button to avoid repeated beeping
    static bool leftBeeped = false;
    static bool middleBeeped = false;
    static bool rightBeeped = false;

    // Check if each button is currently pressed
    bool leftPressed = (digitalRead(PIN_BTN_L) == HIGH);
    bool middlePressed = (digitalRead(PIN_BTN_M) == HIGH);
    bool rightPressed = (digitalRead(PIN_BTN_R) == HIGH);

    // Short press detection for left button
    if (leftPressed) {
        // Check if the button is held beyond the debounce delay
        if ((currentMillis - leftPressStart > buttonDebounceDelay)) {
            if (!leftBeeped) { 
                beep(1000);  // Play beep for a short press
                leftBeeped = true; // Mark as beeped to avoid repeat beeps
                // Handle left short press action here
            }
        }
    } else {
        leftPressStart = currentMillis;  // Reset timer when button is released
        leftBeeped = false;              // Reset beep flag for next press
    }

    // Short press detection for middle button
    if (middlePressed) {
        if ((currentMillis - middlePressStart > buttonDebounceDelay)) {
            if (!middleBeeped) {
                beep(1000);  // Play beep for a short press
                middleBeeped = true; // Mark as beeped to avoid repeat beeps
                // Handle middle short press action here
            }
        }
    } else {
        middlePressStart = currentMillis;  // Reset timer when button is released
        middleBeeped = false;              // Reset beep flag for next press
    }

    // Short press detection for right button
    if (rightPressed) {
        if ((currentMillis - rightPressStart > buttonDebounceDelay)) {
            if (!rightBeeped) {
                beep(1000);  // Play beep for a short press
                rightBeeped = true; // Mark as beeped to avoid repeat beeps
                if (is_display_off) { 
                    display.ssd1306_command(SSD1306_DISPLAYON); // Turn on the display
                    is_display_off = false;    // Update display state
                    beep(1300);  // Additional beep to confirm display is on
                }
            }
        }
    } else {
        rightPressStart = currentMillis;  // Reset timer when button is released
        rightBeeped = false;              // Reset beep flag for next press
    }

    // Long press detection for left button
    if (leftPressed && (currentMillis - leftPressStart > buttonLongPressDelay)) {
        if (!leftBeeped) {
            beep(1000);  // Play beep for a long press
            leftBeeped = true; // Mark as beeped to avoid repeat beeps
            // Handle left long press action here
        }
    }

    // Long press detection for middle button
    if (middlePressed && (currentMillis - middlePressStart > buttonLongPressDelay)) {
        if (!middleBeeped) {
            beep(1000);  // Play beep for a long press
            middleBeeped = true; // Mark as beeped to avoid repeat beeps
            // Handle middle long press action here
        }
    }

    // Long press detection for right button (with display control)
    if (rightPressed && (currentMillis - rightPressStart > buttonLongPressDelay)) {
        if (!is_display_off) { // Turn off the display if it's on
            beep(1300); // Beep to indicate display turn-off
            display.ssd1306_command(SSD1306_DISPLAYOFF); // Turn off the display
            is_display_off = true;     // Update display state
            beep(1000);  // Additional beep to confirm display is off
        }
    }

    // Combined long press detection for left and middle buttons to restart device
    if (leftPressed && middlePressed && 
        (currentMillis - leftPressStart > buttonLongPressDelay) && (currentMillis - middlePressStart > buttonLongPressDelay)) {
            ESP.restart();  // Restart device if both buttons are held long enough
    }
}
