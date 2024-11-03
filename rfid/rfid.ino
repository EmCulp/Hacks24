#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 53
#define RST_PIN 5

MFRC522 rfid(SS_PIN, RST_PIN);

bool isDocked = false;
unsigned long lastDetectedMillis = 0;
const long dockedDuration = 2000; // Duration for docked state

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);  // communication with esp8266

  SPI.begin();
  rfid.PCD_Init();
  Serial.println("RFID reader initialized. Tap card on reader.");

  initializeWiFi();
  checkConnection();
  Serial.println("Connected to WiFi");
}

void loop() {
  if (Serial1.available()) {
      char c = Serial1.read();
      Serial.println(c);  // Print to the Serial Monitor
  }

  unsigned long currentMillis = millis();

  // Check if a new card is present
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    // Card detected for the first time or continuously
    if (!isDocked) {
      // First detection
      Serial.println("Card detected! UID:");
      printUID();
      isDocked = true; // Update state to docked
      lastDetectedMillis = currentMillis; // Update time
    } else {
      // Card is still present
      Serial.print("Card is still present. UID: ");
      printUID();
      lastDetectedMillis = currentMillis; // Update time
    }
  } else {
    // If the card is not new and it was docked, check for removal
    if (isDocked) {
      Serial.println("is present and not new isDocked");
      // Check if the docked duration has passed
      if (currentMillis - lastDetectedMillis > dockedDuration) {
        Serial.println("Card removed, stopping UID print.");
        isDocked = false; // Update state to not docked
      } else {
        // Card is still detected but not new
        Serial.println("Card present, within docked duration.");
      }
    } else {
      Serial.println("No card present.");
    }
  }

  checkForClientRequest();
  Serial.println("done...");
  delay(1000); // Control the loop frequency
}

void printUID() {
  for (int i = 0; i < rfid.uid.size; i++) {
    Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(rfid.uid.uidByte[i], HEX);
  }
  Serial.println();
}

// Initialize Wi-Fi connection
void initializeWiFi() {
  Serial1.println("AT+RST"); // Reset ESP8266
  delay(2000);
  
  Serial1.println("AT+CWMODE=2"); // Set to access point mode
  delay(1000);
  
  Serial1.println("AT+CWJAP=\"YCP-Hacks\",\"Hacks2024\",5"); // Connect to Wi-Fi
  delay(5000); // Wait for connection

  // Check if connected
  Serial1.println("AT+CWJAP?");
  delay(1000);
  while (Serial1.available()) {
    Serial.write(Serial1.read()); // Print any response
  }

  Serial1.println("AT+CIFSR"); // Get IP address
  delay(1000);
  while (Serial1.available()) {
    Serial.write(Serial1.read()); // Print the IP address
  }
  
  Serial1.println("AT+CIPSERVER=1,80"); // Start server
  delay(1000);
  while (Serial1.available()) {
    Serial.write(Serial1.read()); // Print response from server start command
  }
}

void checkForClientRequest() {
  if (Serial1.available()) {
    String request = Serial1.readStringUntil('\r');
    Serial1.read();
    if (request.indexOf("GET / ") >= 0) {
      sendHttpResponse(isDocked ? "Docked" : "Not Docked");
    }
  }
}

// Function to send HTTP response
void sendHttpResponse(String message) {
  Serial1.print("AT+CIPSEND=");
  Serial1.print(message.length() + 4); // Length of message + 4 for headers
  Serial1.println();
  delay(100);
  
  Serial1.println("HTTP/1.1 200 OK"); // Send HTTP status line
  Serial1.println("Content-Type: text/plain"); // Specify content type
  Serial1.println(); // End headers
  Serial1.println(message); // Send the message body
  delay(100);
  
  Serial1.println("AT+CIPCLOSE"); // Close the connection
}

void checkConnection(){
  Serial1.println("AT+CWJAP?"); // check if connected to wifi
  delay(1000);

  while(Serial1.available()){
    Serial.write(Serial1.read());
  }
}
