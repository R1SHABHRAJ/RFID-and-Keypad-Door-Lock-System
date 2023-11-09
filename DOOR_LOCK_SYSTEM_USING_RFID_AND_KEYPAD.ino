//LCD config
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
//#include <LiquidCrystal.h>
//LiquidCrystal lcd(A0, A1, A2, A3, A4, A5);
#include <SPI.h>
#include <MFRC522.h>

#define BUZZER A0 
#define SS_PIN 10
#define RST_PIN 0
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
bool i = true; 
LiquidCrystal_I2C lcd(0x27,16,2);  // set the LCD address to 0x27 for a 16 chars and 2 line display
  String content= "";
#include <Servo.h>
#include <Keypad.h>

#define LED_G A1 //define green LED pin
#define LED_R A2 //define red LED
#define RELAY A3 //relay pin
 char customKey ;
//Variables
int Lock = 90;   //min servo angle  (set yours)
int Unlock = 180; //Max servo angle   (set yours)
int character = 0;
int activated =0;
char Str[16] = {' ', ' ', ' ', ' ', ' ', ' ', '-', '*', '*', '*', ' ', ' ', ' ', ' ', ' ', ' '};  
//int i=0;
//MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance. 

//Pins
Servo myservo;
int buzzer = 13;     //pin for the buzzer beep
int Emergency_open = 12; //pin to inside emergency_open
int Close_Button = 11; //pin to inside Close_Button
 
//Keypad config
const byte ROWS = 4; //four rows
const byte COLS = 4; //four columns
//define the cymbols on the buttons of the keypads
char hexaKeys[ROWS][COLS] = {
  {'1','4','7','*'},
  {'2','5','8','0'},
  {'3','6','9','#'},
  {'A','B','C','D'}
};
byte rowPins[ROWS] = {6, 7, 8, 9}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {5, 4, 3, 2}; //connect to the column pinouts of the keypad
 
//initialize an instance of class NewKeypad
Keypad customKeypad = Keypad( makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 

void(* resetFunc) (void) = 0; // create a standard reset function
 
void setup(){
  Serial.begin(9600);   // Initiate a serial communication
  lcd.init();
  myservo.attach(10); //attach the servo to pin D10
  pinMode(buzzer,OUTPUT); 
  pinMode(Emergency_open,INPUT);
  pinMode(Close_Button,INPUT); 
  lcd.backlight();
  //Init the screen and print the first text
  lcd.begin(16, 2);
  lcd.clear();
  lcd.print("    PASSWORD");
  lcd.setCursor(0,1);
  lcd.print("      -***     ");
  //put the servo in the close position first
  myservo.write(Lock);  
  SPI.begin();          // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  pinMode(LED_G, OUTPUT);
  pinMode(LED_R, OUTPUT);
  pinMode(RELAY, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  noTone(BUZZER);
  digitalWrite(RELAY, HIGH);
  Serial.println("Put your card to the reader...");
  Serial.println();
}

void rfid() {
    // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  //Show UID on serial monitor
  Serial.print("UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
  
  if ((content.substring(1) == "02 CE E5 3F")||(content.substring(1) == "91 A7 F9 1B")) //change here the UID of the card/cards that you want to give access
  {
    if(i==true){
    Serial.println("Authorized access");
    lcd.clear();
    lcd.setCursor(1,0);
    lcd.print("ACCESS GRANTED");
    lcd.setCursor(2,1);
    lcd.print("CARD SCANNED");
    open();
    i=false;
    Serial.println();
    delay(500);
    
    }
    else{
      i=true;
      close();
      return;
    }
  }
 
 else   {
    Serial.println(" Access denied");
    digitalWrite(LED_R, HIGH);
    delay(1000);
    digitalWrite(LED_R, LOW);
   }
}
void open() {
  digitalWrite(RELAY, LOW);
    digitalWrite(LED_G, HIGH);
      lcd.clear();    
      lcd.setCursor(4,0);
      lcd.print("WELCOME");
      delay(500);
      lcd.setCursor(2,1);
      lcd.print("Unlocking...");
      delay(1000);
 
      lcd.clear();    
      lcd.setCursor(3,0);
      lcd.print("DOOR  OPEN");
      lcd.setCursor(3,1);
      lcd.print("Enter  Now");
}
void close() {
  //myservo.write(Lock);
  
      digitalWrite(RELAY, HIGH);
      digitalWrite(LED_G, LOW);
      lcd.clear();
      lcd.setCursor(2,0);
      lcd.print("CLOSING DOOR");
      delay(500);
      Str[6]= '-';
      Str[7]= '*'; 
      Str[8]= '*'; 
      Str[9]= '*';
      Str[10]= ' ';   
      lcd.clear();
      lcd.setCursor(4,0);
      lcd.print("PASSWORD");    
      lcd.setCursor(0,1);
      lcd.print(Str);
      resetFunc();  //call reset
}
void key(){
/////////////////////////KEYPAD OPEN/CLOSE////////////////////////////////////  
  customKey = customKeypad.getKey(); //this function reads the presed key
  
  if (customKey){
   i=2; 
    if (character ==0)
    {  
    Serial.println(customKey);
    Str[6]= customKey;   
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("    PASSWORD");    
    lcd.setCursor(0,1);
    lcd.print(Str);
   
    }
 
    if (character ==1)
    {  
    Serial.println(customKey);
    Str[7]= customKey;   
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("    PASSWORD");    
    lcd.setCursor(0,1);
    lcd.print(Str);
   
    }
 
    if (character ==2)
    {  
    Serial.println(customKey);
    Str[8]= customKey;   
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("    PASSWORD");    
    lcd.setCursor(0,1);
    lcd.print(Str);
   
    }
 
    if (character ==3)
    {  
    Serial.println(customKey);
    Str[9]= customKey;   
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("    PASSWORD");    
    lcd.setCursor(0,1);
    lcd.print(Str);
   
    }
 
    if (character ==4)
    {  
    Serial.println(customKey);
    Str[10]= customKey;
    activated=1;
   
    }
    character=character+1;
  }
 
  if (activated == 1)
    {
/*Change your password below!!! 
Change each of Str[6], Str[7], Str[8], Str[9]*/
 //rfid();
    if((Str[10]='A' && character==5 && Str[6]=='9' && Str[7]=='3' && Str[8]=='6' && Str[9]=='9') )
    {
      //myservo.write(Unlock);
      lcd.clear();
      lcd.setCursor(4,0);
      lcd.print("ACCEPTED");
      activated = 2;
      open();
      
    }
    else
    {
      lcd.clear();    
      lcd.setCursor(1,0);
      lcd.print("PASSWORD ERROR");
      lcd.setCursor(3,1);
      lcd.print("TRY  AGAIN");
      analogWrite(buzzer,150);
      delay(3000);
      analogWrite(buzzer,LOW);
      character=0;
      Str[6]= '-';
      Str[7]= '*'; 
      Str[8]= '*'; 
      Str[9]= '*';
      Str[10]= ' ';
      activated = 0;
      lcd.clear();    
      lcd.setCursor(4,0);
      lcd.print("PASSWORD");
      lcd.setCursor(0,1);
      lcd.print(Str);   
    }
  }
  if (activated == 2)
    {
    if(customKey == 'C' )
    {
     close(); 
     
    }
  }
}
//void resetFunc(void) = 0; //declare reset function @ address 0

void loop(){ 
  rfid();
  key();
  //resetFunc();
}
