#include <HardwareSerial.h>
#include <TinyGPS++.h>

// GPS Setup (RX=GPIO13, TX=GPIO12)
HardwareSerial gpsSerial(2);  // UART2
TinyGPSPlus gps;

void setup() {
  Serial.begin(115200);
  gpsSerial.begin(9600, SERIAL_8N1, 13, 12);  // RX=13, TX=12
  
  Serial.println("GPS Module Test");
  Serial.println("Waiting for satellite lock...");
  Serial.println("-----------------------------------");
}

void loop() {
  // Forward raw GPS data to Serial Monitor
  while (gpsSerial.available() > 0) {
    char c = gpsSerial.read();
    Serial.write(c);  // Raw NMEA sentences
    gps.encode(c);    // Parse data
  }

  // Display parsed data every 2 seconds
  static uint32_t lastMsg = 0;
  if (millis() - lastMsg > 2000) {
    lastMsg = millis();
    
    Serial.println("\nParsed GPS Data:");
    
    if (gps.location.isValid()) {
      Serial.print("Latitude:  ");
      Serial.println(gps.location.lat(), 6);
      
      Serial.print("Longitude: ");
      Serial.println(gps.location.lng(), 6);
      
      Serial.print("Google Maps: https://www.google.com/maps?q=");
      Serial.print(gps.location.lat(), 6);
      Serial.print(",");
      Serial.println(gps.location.lng(), 6);
      
      Serial.print("Satellites: ");
      Serial.println(gps.satellites.value());
      
      Serial.print("Altitude:   ");
      Serial.print(gps.altitude.meters());
      Serial.println(" meters");
      
      Serial.print("Speed:      ");
      Serial.print(gps.speed.kmph());
      Serial.println(" km/h");
    } else {
      Serial.println("INVALID: Waiting for GPS signal...");
    }
    Serial.println("-----------------------------------");
  }
}