// Im not the original author of this code, im just a humble copy/paste wannabe coder.
// Original code creators:
// https://create.arduino.cc/projecthub/kksjunior/windows-pc-lock-unlock-using-rfid-5021a6
// https://devcraze.com/tutorials/arduino/mfrc522-rfid-read-and-write-data-in-specific-rfid-block/
//
// This Sketch requries 4 things to work:
// 1. An arduino that can emulate as a keyboard (Arduino Micro in my case)
// 2. Update your card ID on row 29 (String CardID).
// 3. You need to store first part of your computers password at block 4 on your RFID card.
// 4. Update secound part of your password on row 94 (String PasswordPartTwo).
//
// Please follow theese rules:
// 1. I'm not to blame if anything goes sideways when you're playing around with this sketch.
// 2. Instead of whining about some part of the code or security problem, contribute and make it better.

#include <SPI.h>
#include <MFRC522.h>
#include <Keyboard.h>
#define KEY_RETURN 0xB0

#define RST_PIN         9
#define SS_PIN          10
 
MFRC522 mfrc522(SS_PIN, RST_PIN);       // Create MFRC522 instance
MFRC522::MIFARE_Key key;
MFRC522::StatusCode status;

char Enter = KEY_RETURN;                //Return key is declared as Enter.
String ReadID;
String CardID="00000000";               // Put your cards ID here
 
void setup() {
  Serial.begin(9600);                                           // Initialize serial communications with the PC
  SPI.begin();                                                  // Init SPI bus
  mfrc522.PCD_Init();                                           // Init MFRC522 card
}

//function to store card uid as a string datatype.
void temp(byte *buffer, byte bufferSize) {
  ReadID="";
  for(byte i = 0;i<4; i++) {
    ReadID=ReadID+String(buffer[i], HEX);
  }
}

void loop() {
  // Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
  for (byte i = 0; i < 6; i++) key.keyByte[i] = 0xFF;
 
  byte block;
  byte len;
 
  // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
  if ( ! mfrc522.PICC_IsNewCardPresent()) {
    return;
  }
 
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) {
    return;
  }
 
  Serial.println(F("**Card Detected:**"));
 
  byte buffer1[18];
  block = 4;
  len = 18;
 
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 4, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Authentication failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
 
  status = mfrc522.MIFARE_Read(block, buffer1, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.print(F("Reading failed: "));
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
 
  String PasswordPartOne;
  for (uint8_t i = 0; i < 16; i++) {
      PasswordPartOne += (char)buffer1[i];
  }
  PasswordPartOne.trim();
  Serial.println(PasswordPartOne);                                        //Comment out this after debugging.

  temp(mfrc522.uid.uidByte, mfrc522.uid.size);
  //Serial.println(ReadID);
  
  if(ReadID==CardID) { 
  String PasswordPartTwo = PasswordPartOne + "YourOtherHalfOfThePassword";  // Add your Secound part of the password for pc here
  Serial.println(PasswordPartTwo);
  // Uncomment this part first after your serial monitor gives you the correct password
  /*
  Keyboard.press(KEY_LEFT_GUI);                                             //Press the left windows key.
  Keyboard.press('l');                                                      //Press the "l" key.
  Keyboard.releaseAll();                                                    //Release all keys.
  delay (100);
  Keyboard.press(Enter);                                                    //Press the Enter key.
  Keyboard.release(Enter);                                                  //Release the Enter key.
  delay(100);
  Keyboard.print(PasswordPartTwo);
  Keyboard.releaseAll();
  delay(100);
  Keyboard.press(Enter);
  Keyboard.releaseAll();
  */
  delay(1000);
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  }
  
  else {
  Serial.println("This is wrong card ID");
  Serial.println("Use: " + ReadID + " as Card ID");
  delay(1000);
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  }
  
  delay(1000);
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}
