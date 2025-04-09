#define SIM_RX 16  // ESP32 GPIO16 (RX2) connected to SIM800L TX
#define SIM_TX 17  // ESP32 GPIO17 (TX2) connected to SIM800L RX
#define PWR_PIN 4  // Optional power control pin

String number = "+919188755089"; // Your emergency number

void setup() {
  // Initialize serial communications
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, SIM_RX, SIM_TX); // Hardware Serial for SIM800L
  
  // Power management (if your module has PWRKEY)
  pinMode(PWR_PIN, OUTPUT);
  digitalWrite(PWR_PIN, HIGH);
  delay(1200);  // Critical 1.2s pulse for power on
  digitalWrite(PWR_PIN, LOW);
  delay(2000);  // Allow time for module to initialize

  Serial.println("Initializing SIM800L...");
  
  // Test communication
  sendATCommand("AT", 1000);
  sendATCommand("ATE0", 500);  // Echo off
  sendATCommand("AT+CMEE=1", 500);  // Verbose errors

  Serial.println("Type commands:");
  Serial.println("s - Send SMS");
  Serial.println("r - Receive SMS");
  Serial.println("c - Make call");
}

void loop() {
  // Handle Serial Monitor input
  if (Serial.available()) {
    char command = Serial.read();
    switch (command) {
      case 's': SendMessage(); break;
      case 'r': ReceiveMessage(); break;
      case 'c': CallNumber(); break;
    }
  }

  // Print any responses from SIM800L
  while (Serial2.available()) {
    Serial.write(Serial2.read());
  }
}

// Helper function to send AT commands
void sendATCommand(String cmd, int delayTime) {
  Serial2.println(cmd);
  delay(delayTime);
}

void SendMessage() {
  Serial.println("Sending SMS...");
  
  sendATCommand("AT+CMGF=1", 500);  // Set text mode
  
  String cmd = "AT+CMGS=\"" + number + "\"";
  sendATCommand(cmd, 500);
  
  Serial2.print("Emergency Alert: Accident detected!");  // Message content
  delay(500);
  Serial2.write(26);  // Ctrl+Z to send
  delay(500);
  
  Serial.println("SMS sent!");
}

void ReceiveMessage() {
  Serial.println("Setting up SMS reception...");
  sendATCommand("AT+CMGF=1", 500);  // Text mode
  sendATCommand("AT+CNMI=1,2,0,0,0", 500);  // New SMS notification
  Serial.println("Ready to receive SMS. New messages will appear automatically.");
}

void CallNumber() {
  Serial.println("Calling emergency number...");
  String cmd = "ATD" + number + ";";
  sendATCommand(cmd, 500);
  delay(5000);  // Call duration
  sendATCommand("ATH", 500);  // Hang up
  Serial.println("Call ended.");
}