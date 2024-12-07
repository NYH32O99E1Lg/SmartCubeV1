# SmartCube

**SmartCube** is a compact and customizable desk toy using the ESP8266 D1 Mini. It features a rechargeable power supply, three input buttons, an SSD1306 OLED display for user interface, and a piezo buzzer for sound.  

![SmartCube](https://git.kopic.hr/tomislav/SmartCubeV1/raw/branch/main/hardware/pictures/cubez.jpg)

---

## Features  
- **Microcontroller**: ESP8266 D1 Mini (WeMos)  
- **Display**: SSD1306 OLED (128x64 resolution)  
- **Input**: Three push buttons  
- **Sound**: Piezo buzzer for audio feedback  
- **Power**: 14250 rechargeable battery with TP4056 charging module  
- **Case**: 3D-printable design included in `/hardware` directory  

---

## Bill of Materials  

| **Component**             | **Quantity** | **Description**                           | **Notes**                                   |  
|---------------------------|--------------|-------------------------------------------|---------------------------------------------|  
| ESP8266 D1 Mini           | 1            | Microcontroller module                    | Base of the SmartCube                       |  
| SSD1306 OLED Display      | 1            | 128x64 resolution, I2C interface          | Ensure proper wiring for SDA/SCL            |  
| 14250 Rechargeable Battery| 1            | Lithium-ion battery                       | For powering the cube                       |  
| TP4056 Module             | 1            | Charging and protection circuit           | For battery management                      |  
| Push Buttons              | 3            | Tactile push buttons                      | Used for user input                         |  
| Piezo Buzzer              | 1            | Active buzzer                             | Provides audio feedback                     |  
| Resistors (10kΩ)          | 3            | Pull-down resistors for buttons           | Ensures stable button input                 |  
| Wires / Dupont Connectors | Several      | For connections                           | Ensure reliable wiring                      |  
| Enclosure                 | 1            | 3D-printed or custom-made case            | For housing the components                  |  
| 2x6mm screw               | 12           | Small screws for seembling the case       | Does not have to be exactly 6mm long        |  

---

## Pin Connections  

### OLED Display Settings  
```c  
#define SCREEN_WIDTH 128  
#define SCREEN_HEIGHT 64  
#define OLED_RESET -1  
#define FRAMERATE 8  
```  
The OLED display is connected via I2C to the D1 Mini:  
- **SDA**: D2 (GPIO4)  
- **SCL**: D1 (GPIO5)  

### Buttons and Buzzer  
```c  
#define PIN_BTN_L 12 // D6  
#define PIN_BTN_M 13 // D7  
#define PIN_BTN_R 15 // D8  
#define PIN_BUZZER 0 // D3  
```  
- Button Left (PIN_BTN_L): GPIO12  
- Button Middle (PIN_BTN_M): GPIO13  
- Button Right (PIN_BTN_R): GPIO15  
- Piezo Buzzer (PIN_BUZZER): GPIO0  

---

## Assembly Instructions  

1. **Power Supply Setup**:  
   - Connect the 14250 battery to the TP4056 module for safe charging and protection.  
   - Wire the output of the TP4056 to the **3.3V pin** on the D1 Mini to power the device.  
   - Connect the **5V output pin** from the D1 Mini to the **input port** on the TP4056 module to enable charging functionality.  

2. **OLED Display**: Solder connections for SDA (D2, GPIO4) and SCL (D1, GPIO5) to the respective pins on the D1 Mini.  
3. **Buttons**: Attach each button to the specified GPIO pins with pull-down resistors to ensure reliable input.  
4. **Buzzer**: Connect the piezo buzzer to GPIO0. Ensure proper polarity. Negative to the GPI00, Positive to 3.3v
5. **Enclosure (Optional)**: Assemble all components in a secure housing.  

---

## Demo Code Explanation  

The provided demo code demonstrates the main features of the SmartCube, focusing on system initialization, button handling, OLED display output, and WiFi management. Below is a breakdown of its components:

### Key Features  

1. **WiFi Management**:  
   - The `cubeWifiManager` class manages WiFi connectivity seamlessly.  
   - If no known WiFi network is available, it starts a configuration portal and creates an access point (AP), allowing users to connect the SmartCube to a network from another WiFi-enabled device.  
   - The OLED display shows the access point details, such as the AP name and IP address, making the setup process user-friendly and accessible.

2. **Button Handling**:  
   - Buttons are mapped to specific GPIO pins: `PIN_BTN_L` (Left), `PIN_BTN_M` (Middle), and `PIN_BTN_R` (Right).  
   - The `cubeButtonHandler()` function, executed in the `loop()`, monitors and interprets button presses, differentiating between short and long presses.  

#### Default Button Actions  
- **Right Button**:  
  - *Short Press*: Turns the OLED display **on** (if previously off).  
  - *Long Press*: Turns the OLED display **off**, helping conserve power.  

- **Left + Middle Buttons (Simultaneous Long Press)**:  
  - Triggers an ESP8266 reboot, effectively restarting the device.  

---

## Notes  

- This is still a work in progress
- Ensure all connections are secure to avoid malfunction.  
- Test the circuit thoroughly before placing it in the enclosure.  
- Handle the battery with care to prevent damage.  

For questions or additional details, feel free to reach out!

---