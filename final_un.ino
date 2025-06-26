#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>
#include <WiFi.h>
#include <HTTPClient.h>

#define REPORTING_PERIOD_MS 1000
#define GSR_PIN 34 // GSR Sensor Pin on ESP32

// Threshold values
const int heartRateThreshold = 100;
const float oxygenRateThreshold = 95.0;
const int gsrThreshold = 600;

// WiFi Credentials
const char* ssid = "aaaaaa";
const char* password = "11111111";

// Twilio Credentials
String accountSID = "XXXXXXXXXX";
String authToken = "56fgtrhijkl";
String twilioPhone = "+444444444";
String receiverPhone = "+910000000000";

// Sensor and GPS objects
PulseOximeter pox;
TinyGPSPlus gps;
SoftwareSerial gpsSerial(16, 17); // GPS TX, RX pins
uint32_t lastReportTime = 0;

void onBeatDetected() {
    Serial.println("Beat detected!");
}

void sendSMS(int heartRate, float oxygenRate, int gsrValue, float lat, float lng) {
    HTTPClient http;
    String url = "https://api.twilio.com/2010-04-01/Accounts/" + accountSID + "/Messages.json";

    // Updated message to include heart rate, SpO2, and GSR values
    String message = "🚨 Emergency Alert! Health Risk Detected!\n"
                     "Heart Rate: " + String(heartRate) + " bpm\n"
                     "Oxygen Level: " + String(oxygenRate) + "%\n"
                     "GSR Value: " + String(gsrValue) + "\n"
                     "Location: https://maps.google.com/?q=" + String(lat, 6) + "," + String(lng, 6);

    http.begin(url);
    http.setAuthorization(accountSID.c_str(), authToken.c_str());
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    String body = "To=" + receiverPhone + "&From=" + twilioPhone + "&Body=" + message;
    int httpResponseCode = http.POST(body);

    if (httpResponseCode > 0) {
        Serial.println("SMS Sent Successfully!");
    } else {
        Serial.println("SMS Sending Failed!");
    }
    http.end();
}

void setup() {
    Serial.begin(115200);
    gpsSerial.begin(9600); // Correct baud rate for NEO-6M
    WiFi.begin(ssid, password);

    Serial.println("Connecting to Wi-Fi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.print(".");
    }
    Serial.println("\nWi-Fi Connected");

    if (!pox.begin()) {
        Serial.println("MAX30100 not found! Check wiring.");
        while (1);
    }
    pox.setOnBeatDetectedCallback(onBeatDetected);
    pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);
}

void loop() {
    pox.update();
    int gsrValue = analogRead(GSR_PIN);
    int heartRate = pox.getHeartRate();
    float oxygenRate = pox.getSpO2();

    // Continuously process GPS data
    while (gpsSerial.available()) {
        gps.encode(gpsSerial.read());
    }

    if (millis() - lastReportTime > REPORTING_PERIOD_MS) {
        Serial.print("Heart Rate: ");
        Serial.print(heartRate);
        Serial.print(" bpm | SpO2: ");
        Serial.print(oxygenRate);
        Serial.print("% | GSR: ");
        Serial.println(gsrValue);

        if ((heartRate > heartRateThreshold && gsrValue > gsrThreshold) ||
            (heartRate > heartRateThreshold && oxygenRate < oxygenRateThreshold) ||
            (oxygenRate < oxygenRateThreshold && gsrValue > gsrThreshold)) {

            Serial.println("Emergency Detected!");

            if (gps.location.isValid()) {
                float lat = gps.location.lat();
                float lng = gps.location.lng();
                Serial.print("GPS Location: ");
                Serial.print(lat, 6);
                Serial.print(", ");
                Serial.println(lng, 6);
                sendSMS(heartRate, oxygenRate, gsrValue, lat, lng);
                delay(10000); // Delay to avoid multiple alerts
            } else {
                Serial.println("GPS Signal Not Found! Retrying...");
            }
        } else {
            Serial.println("No Emergency Detected.");
        }

        lastReportTime = millis();
    }
}
