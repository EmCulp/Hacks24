#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>

#define SS_PIN 53
#define RST_PIN 5
#define LED 13  // Define LED pin

MFRC522 rfid(SS_PIN, RST_PIN);

// Update the pin for D4 from the pin 5 to pin 8
LiquidCrystal lcd(12, 11, 8, 4, 3, 2);

bool isDocked = false;
bool nfcDetected = false;
bool uidChanged = false;
unsigned long lastDetectedMillis = 0;
unsigned long lastBlinkMillis = 0;  // Timer for LED blinking
const long dockedDuration = 2000;   // Duration for docked state
const long blinkInterval = 2000;    // 2 seconds for undocked blinking

// Array to hold the last detected UID
byte lastUID[10] = {0};
bool uidPreviouslyDetected = false;  // Track if UID was previously detected

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  Serial.println("RFID reader initialized. Tap card on reader.");

  // LCD setup
  lcd.begin(16, 2);
  lcd.print("NFC System Ready");
  delay(2000);
  lcd.clear();

  // Set up LED
  pinMode(LED, OUTPUT);
}

void loop() {
  bool phoneOnNFC = false;  // Replace with actual phone status
  unsigned long currentMillis = millis();

  // Check if a new card is present
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    nfcDetected = true;

    // Check if the card was previously detected
    if (uidPreviouslyDetected) {
      // Compare current UID with last detected UID
      bool isUIDSame = true;
      for (int i = 0; i < rfid.uid.size; i++) {
        if (rfid.uid.uidByte[i] != lastUID[i]) {
          isUIDSame = false;
          break;
        }
      }

      // If the UID is different, update lastUID and set uidChanged flag
      if (!isUIDSame) {
        for (int i = 0; i < rfid.uid.size; i++) {
          lastUID[i] = rfid.uid.uidByte[i];
        }
        uidChanged = true;  // Set flag that UID has changed
        Serial.println("UID has changed!");
      } else {
        uidChanged = false;  // No change in UID
      }
    } else {
      // First detection, save the UID
      for (int i = 0; i < rfid.uid.size; i++) {
        lastUID[i] = rfid.uid.uidByte[i];
      }
      uidChanged = false;  // No change on first detection
      uidPreviouslyDetected = true;  // Mark UID as detected
      Serial.println("Card detected for the first time!");
    }

    printUID();
    isDocked = true;  // Update state to docked
    lastDetectedMillis = currentMillis;
  } else {
    // Check if the card was docked but has been removed
    if (isDocked && (currentMillis - lastDetectedMillis > dockedDuration)) {
      Serial.println("Card removed, stopping UID print.");
      isDocked = false;
      nfcDetected = false;
      uidPreviouslyDetected = false;  // Reset UID detection status
    }
  }

  // LED Control based on NFC detection and docking state
  if (nfcDetected && !isDocked) {
    // Blink LED every 2 seconds until docked
    if (currentMillis - lastBlinkMillis >= blinkInterval) {
      lastBlinkMillis = currentMillis;
      digitalWrite(LED, !digitalRead(LED));  // Toggle LED state
    }
  } else if (isDocked) {
    // Quick blink LED twice if docked
    for (int i = 0; i < 2; i++) {
      digitalWrite(LED, HIGH);
      delay(50);
      digitalWrite(LED, LOW);
      delay(50);
    }
    // Small delay to prevent continuous quick blinking in each loop iteration
    delay(500);
  } else {
    // Turn off LED if neither condition is met
    digitalWrite(LED, LOW);
  }

  // Update LCD display based on NFC detection and phone status
  lcd.clear();  // Clear the LCD at the beginning of each loop iteration
  lcd.setCursor(0, 0);

  if (nfcDetected) {
    lcd.print("Car unlocked  ");  // Ensure there's space for a second line
    if (uidChanged) {
      lcd.setCursor(0, 1);
      lcd.print("UID Changed!  ");
    } else {
      lcd.setCursor(0, 1);
      lcd.print("UID Unchanged ");  // Indicate that UID hasn't changed
    }
  } else {
    lcd.print("Please scan NFC");  
    lcd.setCursor(0, 1);
    lcd.print("                ");  // Clear second line
  }

  delay(500);  // Control the loop frequency
}

void printUID() {
  for (int i = 0; i < rfid.uid.size; i++) {
    Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(rfid.uid.uidByte[i], HEX);
  }
  Serial.println();
}
