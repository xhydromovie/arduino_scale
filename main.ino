#include "HX711.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

#define DOUT  3
#define CLK  2

HX711 scale;

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

#define BTN_KONTAKTRON1 4
#define BTN_KONTAKTRON2 5
#define BTN_AUTO 6
#define BTN_TARE 7
#define BTN_START 8
#define BTN_BACK 13
#define BTN_PREV 11
#define BTN_NEXT 12
#define BTN_OK 9

#define MOTOR 10

float lastValue = 0;
float tareValue = 15;

int kgAddr = 5;
int factorAddr = 10;

float setKg = 15;
int calibration_factor = 0;

byte kontaktron1State;

byte lastKontaktron1State;

byte menu = 0;

bool work = false;

bool correctRead = false;


void setup() {
  Serial.begin(9600);

  float savedKg;
  EEPROM.get(kgAddr, savedKg);
  
  int savedFactor;
  EEPROM.get(factorAddr, savedFactor);
  
  if (savedKg != 255) {
    setKg = savedKg;
  }
  

  if (savedFactor) {
    calibration_factor = savedFactor;
    Serial.println(calibration_factor);
  }
  
  lcd.begin(16, 2);
  lcd.backlight();

  pinMode(BTN_NEXT, INPUT_PULLUP);
  pinMode(BTN_PREV, INPUT_PULLUP);
  pinMode(BTN_BACK, INPUT_PULLUP);
  pinMode(BTN_OK, INPUT_PULLUP);
  pinMode(BTN_TARE, INPUT_PULLUP);
  pinMode(BTN_AUTO, INPUT_PULLUP);
  pinMode(BTN_START, INPUT_PULLUP);
  pinMode(BTN_KONTAKTRON1, INPUT_PULLUP);
  pinMode(BTN_KONTAKTRON2, INPUT_PULLUP);
  pinMode(MOTOR, OUTPUT);
  

  scale.begin(DOUT, CLK);
  scale.set_scale();
  scale.tare();
}

void loop() {

  if(digitalRead(BTN_OK) == LOW) {
     menuWrapper();
    delay(500);
    while(!digitalRead(BTN_OK));
    
  }

  if(digitalRead(BTN_TARE) == LOW) {

    lcd.clear();
    lcd.home();
    lcd.print("WYTAROWANO");
    scale.tare();

    delay(200);
  }

  displayWeight();
}

void displayWeight() {
  
  scale.set_scale(calibration_factor);
  float readKg = scale.get_units(4);

  lcd.setCursor(0,0);
  lcd.print("Docelowa: ");
  lcd.setCursor(11, 0);
  lcd.print(setKg);

  lcd.setCursor(0, 1);
  lcd.print("Waga: ");

  if(lastValue < 0) {
    lcd.print("          ");
  }
  
  lcd.setCursor(6, 1);
  lcd.print(readKg);

  lastValue = readKg;

  if(!digitalRead(BTN_START)) {
      work = true;
      delay(200);
      while(!digitalRead(BTN_PREV));
  }

  kontaktron1State = digitalRead(BTN_KONTAKTRON1);

  if (kontaktron1State != lastKontaktron1State) {
    if (kontaktron1State == LOW) {
      work = true;
    }
    delay(50);
  }
  
  lastKontaktron1State = kontaktron1State;

  if(correctRead == false && work == true) {
    if (readKg > 11 && readKg < 13) {
      correctRead = true;
    }
//    if (readKg > 12) {  // optional
//      corrrectRead = true;
//    }
  }

  
  
  if(!digitalRead(BTN_AUTO) && work == true) {
    analogWrite(MOTOR, 245);

    if(readKg > setKg - 2) {
      analogWrite(MOTOR, 200);
    }
    
    // Zatrzymywanie tasmy
    if(readKg > setKg && correctRead == true) {
      analogWrite(MOTOR, 0);
      correctRead = false;
      work = false;
    }
  }
    
  if(digitalRead(BTN_AUTO)) {
    analogWrite(MOTOR, 0);
    work = false;
  }
}

void menuWrapper() {
  lcd.clear();
  menu = 0;
  updateMenu();
  
  bool back = false;
  
  while(!back) {
    if(!digitalRead(BTN_BACK)) {
      back = true;
      break;
    }

    if(!digitalRead(BTN_PREV)) {
      menu++;
      updateMenu();
      delay(200);
      while(!digitalRead(BTN_PREV));
    }

    if (!digitalRead(BTN_NEXT)){
      menu--;
      updateMenu();
      delay(200);
      while(!digitalRead(BTN_NEXT));
    }

    if(!digitalRead(BTN_OK)) {
      executeAction();
      delay(300);
      while(!digitalRead(BTN_OK));
    }
  }

  lcd.clear();
}

void updateMenu() {
  switch (menu) {
    case 0:
      lcd.clear();
      lcd.print("  Ustaw wage");
      lcd.setCursor(0, 1);
      lcd.print("  Skalibruj wage");
      break;
    case 1:
      lcd.clear();
      lcd.print("> Ustaw wage");
      lcd.setCursor(0, 1);
      lcd.print("  Skalibruj wage");
      break;
    case 2:
      lcd.clear();
      lcd.print("  Ustaw wage");
      lcd.setCursor(0, 1);
      lcd.print("> Skalibruj wage");
      break;
    case 3:
      menu = 2;
      break;
  }
}

void executeAction() {
  switch (menu) {
    case 1:
      setWeight();
      break;
    case 2:
      calibrate();
      break;
  }
}

void setWeight() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Ustaw wage");
  lcd.setCursor(0, 1);
  lcd.print("Zadana: ");
 
  bool back = false;
  
  while(!back) {

    lcd.setCursor(8, 1);
    lcd.print(setKg);

    if(!digitalRead(BTN_BACK)) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("ZAPISANO");
      EEPROM.put(kgAddr, setKg);
      
      delay(800);
      
      back = true;
      break;
    }

    if(!digitalRead(BTN_PREV)) {
      setKg = setKg + 0.1;
      delay(200);
      while(!digitalRead(BTN_PREV));
    }

    if (!digitalRead(BTN_NEXT)){
      setKg = setKg - 0.1;
      delay(200);
      while(!digitalRead(BTN_NEXT));
    }
  }

  lcd.clear();
  updateMenu();
}

void calibrate() {
  bool back = false;
  
  lcd.clear();
  lcd.home();
  lcd.print("Umiesc 15KG");
  lcd.setCursor(0, 1);
  lcd.print("Kliknij DALEJ");

  while(!back) {
    
    if(!digitalRead(BTN_BACK)) {
      back = true;
      updateMenu();
      break;
    }
    
    if(!digitalRead(BTN_PREV)) {
        cal();
        delay(200);
        while(!digitalRead(BTN_PREV));
    }
  }
}

void cal() {
  lcd.clear();
  
  bool nieUstawiono = true;
  
  while(nieUstawiono = true) {
    float readKg = scale.get_units(1);

    lcd.setCursor(0, 0);
    lcd.print(readKg);

    lcd.setCursor(0, 1);
    lcd.print(calibration_factor);
    
    if(readKg > tareValue - 0.01 && readKg < tareValue + 0.01) {
      lcd.clear();
      lcd.print("Skalibrowano!");
      EEPROM.put(factorAddr, calibration_factor);
      delay(1000);
      nieUstawiono = false;
      break;
    }

    if(readKg > tareValue) {
      calibration_factor = calibration_factor + 20;
    } else if (readKg < tareValue) {
      calibration_factor = calibration_factor - 20;
    }
    
    scale.set_scale(calibration_factor);
  }
  
  lcd.clear();

  
  updateMenu();
}
