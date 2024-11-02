/*
 * Created by ArduinoGetStarted.com
 *
 * This example code is in the public domain
 *
 * Tutorial page: https://arduinogetstarted.com/tutorials/arduino-rfid-nfc
 */

#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 5

MFRC522 rfid(SS_PIN, RST_PIN);

void setup() {
  Serial.begin(9600);
  Serial.println("Starting setup...");
  SPI.begin();
  
  // // Self-test
  // Serial.println("Performing self-test...");
  // if (!rfid.PCD_PerformSelfTest()) {
  //   Serial.println("RFID module failed self-test. Check connections and power.");
  //   while (true); // Stop if self-test fails
  // }
  
  Serial.println("Initializing RFID reader...");
  rfid.PCD_Init();
  Serial.println("RFID reader initialized. Tap card on reader.");
}



void loop() {
  Serial.println("Checking for new card...");
  if (rfid.PICC_IsNewCardPresent()) { 
    Serial.println("Card detected!");

    if (rfid.PICC_ReadCardSerial()) { 
      Serial.println("UID read successfully:");
      MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);

      Serial.print("UID:");
      for (int i = 0; i < rfid.uid.size; i++) {
        Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(rfid.uid.uidByte[i], HEX);
      }
      Serial.println();

      rfid.PICC_HaltA(); 
      rfid.PCD_StopCrypto1();
    }
    docked();
    return;
  } else {
    Serial.println("No card present.");
  }
  delay(5000); // Add a delay to avoid spamming the Serial Monitor
}

void docked(){
}