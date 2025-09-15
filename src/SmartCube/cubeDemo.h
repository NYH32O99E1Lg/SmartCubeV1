void interactiveCubeDemo() {
    const int cx = SCREEN_WIDTH / 2;
    const int cy = SCREEN_HEIGHT / 2;
    const int size = 15;  // small cube
    float angle = 0;
    float speed = 0.05;
    bool rotating = true;
    bool showVersion = false;

    bool lastMiddleState = LOW; // Track previous middle button state

    while (true) {
        display.clearDisplay();

        // Read buttons
        bool leftPressed   = digitalRead(PIN_BTN_L) == HIGH;
        bool rightPressed  = digitalRead(PIN_BTN_R) == HIGH;
        bool middlePressed = digitalRead(PIN_BTN_M) == HIGH;

        // Button actions
        if (leftPressed) { 
            speed = -0.05; 
            beep(1000);
        }
        if (rightPressed) { 
            speed = 0.05; 
            beep(1000); 
        }
        // Detect middle button press event
        if (middlePressed && !lastMiddleState) { 
            rotating = !rotating;       // toggle rotation
            showVersion = !rotating;      // show image if stopped
            beep(1000); 
        }
        lastMiddleState = middlePressed; // update previous state

        // Draw either image or cube
        if (showVersion) {
            display.clearDisplay();
            display.setTextSize(2);
            display.setCursor(10, 0);
            display.println("SmartCube");
            display.setTextSize(1);
            display.setCursor(40, 18); 
            display.println("version");
            display.setCursor(52, 28);
            display.println("1.1");
            display.drawLine(0, 40, SCREEN_WIDTH, 40, WHITE);
            display.setCursor(0, 44);
            display.println("by besna_shnita");
            display.setCursor(18, 55);
            display.println("tomislav@kopic.hr");
        } else {
            if (rotating) angle += speed;

            float cosA = cos(angle);
            float sinA = sin(angle);

            int x[8], y[8];
            int vx[8] = {-size, size, size, -size, -size, size, size, -size};
            int vy[8] = {-size, -size, size, size, -size, -size, size, size};
            int vz[8] = {-size, -size, -size, -size, size, size, size, size};

            for (int i = 0; i < 8; i++) {
                float x3d = vx[i] * cosA - vz[i] * sinA;
                float z3d = vx[i] * sinA + vz[i] * cosA;
                float y3d = vy[i];
                float factor = 50.0 / (z3d + 50.0);
                x[i] = cx + x3d * factor;
                y[i] = cy + y3d * factor;
            }

            int edges[12][2] = {
                {0,1},{1,2},{2,3},{3,0},
                {4,5},{5,6},{6,7},{7,4},
                {0,4},{1,5},{2,6},{3,7}
            };
            for (int i = 0; i < 12; i++)
                display.drawLine(x[edges[i][0]], y[edges[i][0]],
                                 x[edges[i][1]], y[edges[i][1]], WHITE);
        }

        display.display();
        delay(50);

        // Exit demo if all buttons pressed
        if (leftPressed && middlePressed && rightPressed) break;
    }
}
