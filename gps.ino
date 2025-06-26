#include <TinyGPS++.h>
#include <HardwareSerial.h>

static const int RXPin = 16, TXPin = 17; // Connect to GPS TX, RX respectively
static const uint32_t GPSBaud = 9600;   // Default baud rate for NEO-6M

TinyGPSPlus gps;
HardwareSerial mySerial(1); // Use Serial1

void setup() {
    Serial.begin(115200);
    mySerial.begin(GPSBaud, SERIAL_8N1, RXPin, TXPin);
    Serial.println("GPS module testing...");
}

void loop() {
    while (mySerial.available() > 0) {
        gps.encode(mySerial.read());
        if (gps.location.isUpdated()) {
            Serial.print("Latitude: ");
            Serial.print(gps.location.lat(), 6);
            Serial.print(" Longitude: ");
            Serial.println(gps.location.lng(), 6);
        }
    }
}
