
#include <SPI.h> 
#include <RFID.h>
#include <Servo.h> 
#include <LiquidCrystal.h>
LiquidCrystal lcd(8, 7, 6, 5, 4, 2);
RFID rfid(10, 9);       
unsigned char status; 
unsigned char str[MAX_LEN]; //MAX_LEN is 16: size of the array 

String accessGranted [2] = {"1138713418", "19612012715"};  
int accessGrantedSize = 2;                                

Servo lockServo;                
int lockPos = 75;               //Locked position limit
int unlockPos = 180;             //Unlocked position limit
boolean locked = true;

int redLEDPin = 5;
int greenLEDPin = 6;

void setup() 
{ 
  Serial.begin(9600);  
  lcd.begin(16, 2);                 //tell the lcd library that we are using a display that is 16 characters wide and 2 characters high
  lcd.clear(); //clear the display   
  SPI.begin();            
  rfid.init();            
  pinMode(redLEDPin, OUTPUT);     
  pinMode(greenLEDPin, OUTPUT);
  digitalWrite(redLEDPin, HIGH);
  delay(200);
  digitalWrite(greenLEDPin, HIGH);
  delay(200);
  digitalWrite(redLEDPin, LOW);
  delay(200);
  digitalWrite(greenLEDPin, LOW);
  lockServo.attach(3);
  lockServo.write(lockPos);         //Move servo into locked position
  Serial.println("Place card/tag near reader...");
  lcd.setCursor(0, 0);
  lcd.print("Place Card");
  lcd.setCursor(0, 1);
  lcd.print("On Reader");
} 

void loop() 
{ 
  if (rfid.findCard(PICC_REQIDL, str) == MI_OK)   //Wait for a tag
  { 
    Serial.println("Card found"); 
    String temp = "";                             
    if (rfid.anticoll(str) == MI_OK)               
    { 
      Serial.print("The card's ID number is : "); 
      for (int i = 0; i < 4; i++)                 
      { 
        temp = temp + (0x0F & (str[i] >> 4)); 
        temp = temp + (0x0F & str[i]); 
      } 
      Serial.println (temp);
      checkAccess (temp);    
    } 
    rfid.selectTag(str); 
  }
  rfid.halt();
}

void checkAccess (String temp)    //Function to check if an identified tag is registered to allow access
{
  boolean granted = false;
  for (int i=0; i <= (accessGrantedSize-1); i++)    //Runs through all tag ID numbers registered in the array
  {
    if(accessGranted[i] == temp)            //If a tag is found then open/close the lock
    {
      Serial.println ("Access Granted");
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Access Granted");
      delay(200);
      granted = true;
      if (locked == true)         //If the lock is closed then open it
      {
          lockServo.write(unlockPos);
          locked = false;
      }
      else if (locked == false)   //If the lock is open then close it
      {
          lockServo.write(lockPos);
          locked = true;
      }
      digitalWrite(greenLEDPin, HIGH);    //Green LED sequence
      delay(200);
      digitalWrite(greenLEDPin, LOW);
      delay(200);
      digitalWrite(greenLEDPin, HIGH);
      delay(200);
      digitalWrite(greenLEDPin, LOW);
      delay(200);
    }
  }
  if (granted == false)     //If the tag is not found
  {
    Serial.println ("Access Denied");
    lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Access Denied");
    digitalWrite(redLEDPin, HIGH);      //Red LED sequence
    delay(200);
    digitalWrite(redLEDPin, LOW);
    delay(200);
    digitalWrite(redLEDPin, HIGH);
    delay(200);
    digitalWrite(redLEDPin, LOW);
    delay(200);
  }
}
