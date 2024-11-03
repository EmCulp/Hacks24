#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>

#define SS_PIN 53
#define RST_PIN 5

MFRC522 rfid(SS_PIN, RST_PIN);

//update the pin for D4 from the pin 5 to pin 8
LiquidCrystal lcd(12, 11, 8, 4, 3, 2);

bool isDocked = false;
unsigned long lastDetectedMillis = 0;
const long dockedDuration = 2000;  // Duration for docked state

//array to hold the last detected UID
byte lastUID[10] = {0}; //size should be large enough for UIDs
bool uidChanges = false;

void setup() {
  Serial.begin(9600);

  SPI.begin();
  rfid.PCD_Init();
  Serial.println("RFID reader initialized. Tap card on reader.");

  //lcd set up
  lcd.begin(16, 2);
  lcd.print("NFC System Ready");
  delay(2000);
  lcd.clear();
}

void loop() {

  bool nfcDetected = false;  //will replace with actual NFC status
  bool phoneOnNFC = false;   //replace with actual NFC phone status

  unsigned long currentMillis = millis();

  // Check if a new card is present
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    nfcDetected = true;
    //compare cuurent UID with last detected UID
    bool is UIDSame = true;


    // Check if the UID has changed
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
      Serial.print("Card is still present. UID: ");
    }
    
    printUID();
    isDocked = true;                     // Update state to docked
    lastDetectedMillis = currentMillis;  // Update time
  } else {
    // If the card is not new and it was docked, check for removal
    if (isDocked) {
      if (currentMillis - lastDetectedMillis > dockedDuration) {
        Serial.println("Card removed, stopping UID print.");
        isDocked = false;     // Update state to not docked
        nfcDetected = false;  // Reset NFC detection status
      }
    }
  }

  //update LCD display based on NFC detection and phone status
  lcd.setCursor(0, 0);
  if (nfcDetected) {
    lcd.print("Car unlocked");
    if(uidChanged){
      lcd.setCursor(0, 1);
      lcd.print("UID Changed!");
    }
  } else {
    lcd.print("Please scan NFC");
    lcd.setCursor(0, 1);
    lcd.print("             ");
  }

  //simulate phone detection
  lcd.setCursor(0, 1);
  if (phoneOnNFC) {
    lcd.print("Drive Enabled");
  } else {
    lcd.print("No phone detected");
  }

  delay(500);
  lcd.clear();

  delay(1000);  // Control the loop frequency
}

void printUID() {
  for (int i = 0; i < rfid.uid.size; i++) {
    Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(rfid.uid.uidByte[i], HEX);
  }
  Serial.println();
}
