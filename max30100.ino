#include <Wire.h>
#include "MAX30100_PulseOximeter.h"

#define REPORTING_PERIOD_MS 1000  // Reporting interval

PulseOximeter pox;
uint32_t lastReportTime = 0;

void onBeatDetected() {
    Serial.println("Beat detected!");
}

void setup() {
    Serial.begin(115200);
    Serial.println("Initializing Pulse Oximeter...");

    if (!pox.begin()) {
        Serial.println("MAX30100 not found! Check wiring.");
        while (1);
    }

    pox.setOnBeatDetectedCallback(onBeatDetected);
    pox.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);  // Adjust LED current if needed
}

void loop() {
    pox.update();
    
    if (millis() - lastReportTime > REPORTING_PERIOD_MS) {
        Serial.print("Heart Rate: ");
        Serial.print(pox.getHeartRate());
        Serial.print(" bpm | SpO2: ");
        Serial.print(pox.getSpO2());
        Serial.println(" %");
        lastReportTime = millis();
    }
}
