#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 5

MFRC522 rfid(SS_PIN, RST_PIN);

bool isDocked = false;
unsigned long lastDetectedMillis = 0;
const long dockedDuration = 2000; // Duration for docked state

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  Serial.println("RFID reader initialized. Tap card on reader.");
}

void loop() {
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

  delay(1000); // Control the loop frequency
}

void printUID() {
  for (int i = 0; i < rfid.uid.size; i++) {
    Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(rfid.uid.uidByte[i], HEX);
  }
  Serial.println();
}
