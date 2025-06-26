#define GSR_PIN 34  // Define the ADC pin connected to the GSR sensor's OUT pin

void setup() {
  Serial.begin(115200);  // Start serial communication at 115200 baud rate
  pinMode(GSR_PIN, INPUT); // Set the GSR_PIN as an input
}

void loop() {
  // Read the analog value from the GSR sensor
  int rawValue = analogRead(GSR_PIN);
  
  // If using a voltage divider, scale the value back to match the original signal
  float scaledValue = rawValue * 2.0; // Multiply by 2 if you're halving the voltage
  
  // Convert to voltage (optional, depends on your use case)
  float voltage = (scaledValue / 4095.0) * 3.3; // 12-bit ADC range (0-4095), ESP32 ADC ref voltage = 3.3V
  
  // Print the readings to the Serial Monitor
  Serial.print("Raw ADC Value: ");
  Serial.print(rawValue);
  Serial.print(" | Scaled Value: ");
  Serial.print(scaledValue);
  Serial.print(" | Voltage: ");
  Serial.print(voltage, 2); // Print voltage with 2 decimal places
  Serial.println(" V");
  
  delay(500); // Delay for stability
}

