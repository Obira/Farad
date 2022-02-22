//#include <Arduino.h>
//start with the lCD then the rfid

/*For the LCD use these details

const int rs=7, en=9, d4=5, d5=4, d6=3, d7=2;
vss goes to Ground.
VDD goes to 5V
v0 goes to middle pin of potentiometer
rs goes to pin 7.
rw goes to ground.
e goes to pin 9.
k(cathode) goes to ground.
A(Anode) goes to 5V via resistor.
d7 goes to pin 2
d6 goes to pin 3
d5 goes to pin 4
d4 goes to pin 5

*/

//For the rfid there is need connection on pin 6 instead of 9 coz it is consumed by LCD
//for the rfid we use the connection for the dump info shown below.
/*
 * Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno/101       Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          6             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 * ground goes to ground
 * 3.3 volts goes to 3.3 volts of arduino
 
 //servo connection
 The orange is signal pin goes to A5
 The red wire goes to 5V
 The Brown wire goes to Ground
 
 //The bazzer
 The long leg goes to A1.
 The shorter leg to ground.
 
 The potentiometer
 The Middle one goes to VO of LCD.
 The one on the Right hand goes to ground
 The one on the left goes to 5V.
 
 //The Green led
 The positive of led goes to A0 via Resistor.
 The shorter leg goes to ground.
 
 //The Red led
 The positive of led goes to pin 8 via Resistor.
 The shorter leg goes to ground.
 
 */


#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>
#include <Servo.h>

#define RST_PIN 6
#define SS_PIN 10
#define ALARM A1
#define LED_GREEN A0
#define LED_RED 8
#define SERVO_PIN A5

Servo myServo;
int pos = 0;// Initial position

byte readCard[4];
String MasterTag = "77ADB3C"; // Master tag saved in MCU memory
//String MasterTag = "EBD4221B";

String tagID = ""; 


// create obj mfrc522, lcd
MFRC522 mfrc522(SS_PIN,RST_PIN);
//LiquidCrystal lcd(7,6,5,4,3,2); // (rs, en,d4,d5,d6,d7)

const int rs=7, en=9, d4=5, d5=4, d6=3, d7=2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//boolean getID();
void setup() {
  Serial.begin(9600);    // Initialize serial communications with the PC
  while (!Serial);
  SPI.begin(); //SPI bus init
  mfrc522.PCD_Init();
  lcd.begin(16,2);
  lcd.clear();
  lcd.print("Door Control");
  lcd.setCursor(0,1);
  lcd.print("Scan now >>");

 //DECLARE ALARM AS OUTPUT
 pinMode(ALARM, OUTPUT);
 pinMode(LED_GREEN, OUTPUT);
 pinMode(LED_RED, OUTPUT);
myServo.attach(A5);//connect servo to pin 6
myServo.write(pos);//Initial position
 
}

void loop() {
  while (getID()) {
   
    if (tagID == MasterTag) {
       lcd.clear();
    lcd.setCursor(0,0);
       lcd.print(" ID: ");
      
  lcd.print(tagID);
  lcd.setCursor(0,1);
     lcd.print("Access Granted");
     digitalWrite(LED_GREEN, HIGH);
       delay(1000);
     digitalWrite(LED_GREEN, LOW);
      
        
      // open door (sevo motor angle 0 to 90, delay), lightning LED,
      // turn on alarm
for (pos = 0; pos <= 90; pos += 1) { // goes from 0 degrees to 90 degrees
    // in steps of 1 degree
    myServo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);
}

delay(5000);
     for (pos = 90; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    myServo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }

   delay(5000); 
  
   lcd.clear();
  lcd.print("Door Control");
  lcd.setCursor(0,1);
   lcd.println("Scan now >>>");
      
    } else {
        lcd.clear();
    lcd.setCursor(0,0);
       lcd.print(" ID: ");
      
  lcd.print(tagID);
  lcd.setCursor(0,1);
     lcd.print("Access denied");
       digitalWrite(LED_RED, HIGH);
     tone(ALARM,2000);
    delay(1000);
    digitalWrite(LED_RED, LOW);
     noTone(ALARM);
     delay(5000); 
  
   lcd.clear();
  lcd.print("Door Control");
  lcd.setCursor(0,1);
   lcd.println("Scan now >>>");

 
  
    }
  /*lcd.setCursor(0,1);
  lcd.print(" ID: ");
        Serial.println("ID: ");

  lcd.print(tagID);
  Serial.println(tagID);

  delay(2000);
 lcd.clear();
  lcd.print("Door Control");
  Serial.println("Door Control");

 //
  lcd.setCursor(0,1);
  lcd.print("Scan now >>");
  Serial.println("Scan now >>");*/
  }
}

// user define function
boolean getID() {
  // check for the new card
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return false;
  }
  if (!mfrc522.PICC_ReadCardSerial()) {
    return false;
  }
  tagID = "";
  for (uint8_t i = 0; i < 4; i++) {
    tagID.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  tagID.toUpperCase();
  // stop read
  mfrc522.PICC_HaltA();
  return true;
}
