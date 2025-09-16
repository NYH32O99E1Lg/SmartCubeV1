#define ADC_PIN A0  // ESP8266 ADC pin

// Tuneable min and max battery voltages
float VOLTAGE_MAX = 4.2;  // Full battery voltage
float VOLTAGE_MIN = 3.0;  // Empty battery voltage

const float R1 = 220000.0;  // 220k ohms
const float R2 = 56000.0;   // 56k ohms
const float ADC_MAX = 1023.0; // 10-bit ADC resolution
const float V_REF = 1.0;      // ESP8266 ADC reference voltage

float readBatteryVoltage() {
  int rawADC = analogRead(ADC_PIN);
  float voltage = (rawADC / ADC_MAX) * V_REF;
  return voltage * (R1 + R2) / R2; // Scale to actual battery voltage
}

int batteryPercentage(float voltage) {
  if (voltage >= VOLTAGE_MAX) return 100;
  if (voltage <= VOLTAGE_MIN) return 0;
  return (voltage - VOLTAGE_MIN) * 100 / (VOLTAGE_MAX - VOLTAGE_MIN); // Linear mapping
}
