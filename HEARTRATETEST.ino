#include <WiFi.h>
#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"

MAX30105 particleSensor;

// WiFi Credentials
const char* ssid = "Sughalle mwone";
const char* password = "noeljoyce";

WiFiServer server(80);
WiFiClient client;

// Sensor Variables
const byte RATE_SIZE = 4;
byte rates[RATE_SIZE];
byte rateSpot = 0;
long lastBeat = 0;
float beatsPerMinute;
int beatAvg;
int spo2;

void setup() {
  Serial.begin(115200);
  Wire.begin(18, 19); // SDA=GPIO18, SCL=GPIO19

  // Initialize MAX30102
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println("MAX30102 not found!");
    while (1);
  }

  // Configure Sensor
  particleSensor.setup();
  particleSensor.setPulseAmplitudeRed(0x1F);
  particleSensor.setPulseAmplitudeIR(0x1F);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi Connected! IP: " + WiFi.localIP());
  server.begin();
}

void loop() {
  client = server.available();
  if (client) {
    Serial.println("MATLAB Connected!");

    while (client.connected()) {
      long irValue = particleSensor.getIR();
      long redValue = particleSensor.getRed();

      if (irValue > 50000) { // Finger detected
        // Heart Rate Calculation
        if (checkForBeat(irValue)) {
          long delta = millis() - lastBeat;
          lastBeat = millis();
          beatsPerMinute = 60 / (delta / 1000.0);
          
          if (beatsPerMinute < 255 && beatsPerMinute > 20) {
            rates[rateSpot++] = (byte)beatsPerMinute;
            rateSpot %= RATE_SIZE;
            beatAvg = 0;
            for (byte x = 0; x < RATE_SIZE; x++)
              beatAvg += rates[x];
            beatAvg /= RATE_SIZE;
          }
        }

        // SpO2 Estimation (simplified)
        float ratio = (float)redValue / irValue;
        spo2 = 104 - 17 * ratio;

        // Simulate EEG signal (replace with actual EEG if available)
        float eegSim = random(80, 100) * (1 + sin(millis() / 1000.0));

        // Send to MATLAB: "BPM,SpO2,EEG_SIM,IR,Red"
        client.print(beatAvg);
        client.print(",");
        client.print(spo2);
        client.print(",");
        client.print(eegSim);
        client.print(",");
        client.print(irValue);
        client.print(",");
        client.println(redValue);
        
        Serial.printf("BPM: %d | SpO2: %d%% | EEG: %.1f\n", beatAvg, spo2, eegSim);
      } 
      else {
        client.println("0,0,0,0,0"); // No finger
      }
      delay(20); // 50Hz sampling
    }
    client.stop();
    Serial.println("Client disconnected");
  }
}