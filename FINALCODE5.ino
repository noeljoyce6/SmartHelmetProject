#include <WiFi.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <MAX30105.h>
#include "Adafruit_MPU6050.h"
#include "Adafruit_Sensor.h"
#include <TinyGPSPlus.h>

// Wi-Fi credentials
const char* ssid = "Sughalle mwone";
const char* password = "noeljoyce";

// IP for MATLAB to connect (ESP will act as server)
WiFiServer server(80);

// Modules
MAX30105 particleSensor;
Adafruit_MPU6050 mpu;
TinyGPSPlus gps;

// GPS Serial
HardwareSerial GPSModule(1);
#define GPS_TX 12
#define GPS_RX 13

// SIM800L Serial
SoftwareSerial sim800(17, 16); // TX, RX

// Sensor Pins
#define MQ3_PIN 34
#define IR_PIN 14
#define BUZZER_PIN 25

// Thresholds
int alcoholThreshold = 500;
int fallThreshold = 30;
int heartRateThreshold = 50;
int spo2Threshold = 90;

// Emergency contacts
String emergencyNumbers[] = {
  "+919188755089", "+919061645091", "+918590607872"
};

// Variables
int heartRate = 0;
int spo2 = 0;
int eegSim = 0;
bool fallDetected = false;
bool helmetWorn = false;

void setup() {
  Serial.begin(115200);
  Wire.begin();

  pinMode(IR_PIN, INPUT);
  pinMode(MQ3_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Start GPS
  GPSModule.begin(9600, SERIAL_8N1, GPS_TX, GPS_RX);

  // Start SIM800
  sim800.begin(9600);

  // Initialize MPU6050
  if (!mpu.begin()) {
    Serial.println("MPU6050 not found!");
    while (1);
  }

  // Initialize MAX30105
  if (!particleSensor.begin(Wire, I2C_SPEED_STANDARD)) {
    Serial.println("MAX30105 not found!");
    while (1);
  }
  particleSensor.setup(); // Use default

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  Serial.println("WiFi connected");
  Serial.println(WiFi.localIP());

  // Start server
  server.begin();
}

void loop() {
  WiFiClient client = server.available();

  // Helmet detection
  helmetWorn = digitalRead(IR_PIN) == HIGH;

  // Alcohol detection
  int alcoholLevel = analogRead(MQ3_PIN);

  // MPU6050 fall detection
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);
  float acc = sqrt(a.acceleration.x * a.acceleration.x +
                   a.acceleration.y * a.acceleration.y +
                   a.acceleration.z * a.acceleration.z);
  fallDetected = (acc > fallThreshold);

  // MAX30105 readings
  long ir = particleSensor.getIR();
  long red = particleSensor.getRed();
  heartRate = map(red % 1000, 0, 1000, 60, 100);
  spo2 = map(ir % 1000, 0, 1000, 90, 100);
  eegSim = random(50, 100); // Fake EEG

  // Prepare data line for MATLAB
  String data = "DATA," + String(heartRate) + "," + String(spo2) + "," + String(eegSim);
  if (client) {
    client.println(data);
    client.flush();
  }

  // Emergency condition
  if (fallDetected || alcoholLevel > alcoholThreshold || heartRate < heartRateThreshold || spo2 < spo2Threshold) {
    digitalWrite(BUZZER_PIN, HIGH);
    sendSMS(alcoholLevel);
    delay(3000);
  } else {
    digitalWrite(BUZZER_PIN, LOW);
  }

  delay(500); // Adjust sampling rate
}

void sendSMS(int alcoholLevel) {
  String status = (heartRate > heartRateThreshold && spo2 > spo2Threshold && alcoholLevel < alcoholThreshold) ? "Person is fine." : "Person is not fine.";

  // Get GPS location
  String gpsLink = "";
  while (GPSModule.available()) {
    gps.encode(GPSModule.read());
    if (gps.location.isUpdated()) {
      gpsLink = "https://maps.google.com/?q=" + String(gps.location.lat(), 6) + "," + String(gps.location.lng(), 6);
      break;
    }
  }

  String msg = status + "\nHeart Rate: " + String(heartRate) +
               "\nSpO2: " + String(spo2) +
               "\nAlcohol: " + String(alcoholLevel) +
               "\nLocation: " + gpsLink;

  for (String number : emergencyNumbers) {
    sim800.println("AT+CMGF=1");
    delay(100);
    sim800.println("AT+CMGS=\"" + number + "\"");
    delay(100);
    sim800.print(msg);
    delay(100);
    sim800.write(26); // Ctrl+Z
    delay(3000);
  }
}
