#include "HX711.h"

#define DOUT2 5
#define CLK2 4
#define DOUT  3
#define CLK  2

HX711 scale;
HX711 scale2;

float tareValue = 1.0;
float calibration_factor = -43050;
//float calibration_factor = -10000;//-7050 worked for my 440lb max scale setup

void setup() {
  Serial.begin(9600);
  
  scale.begin(DOUT, CLK);
  scale.set_scale(calibration_factor);
  Serial.println("Ustawiono factor - belka lewa");
  delay(500);
  scale.tare();
  Serial.println("Wytarowano - belka lewa");

  scale2.begin(DOUT2, CLK2);
  scale2.set_scale(calibration_factor);
  Serial.println("Ustawiono factor - belka prawa");
  delay(500);
  scale2.tare();
  Serial.print("Wytarowano - belka lewa");
  
}

void loop() {
  
  float readKg = scale.get_units(2);
  float readKg2 = scale2.get_units(2);

  Serial.print("Belka prawa: ");
  Serial.println(readKg2);

  Serial.print("Belka lewa: ");
  Serial.println(readKg);

  Serial.print("Suma pomiarow: ");
  Serial.println(readKg + readKg2);

  Serial.print("Srednia pomiarow: ");
  Serial.println((readKg + readKg2) / 2 );
  Serial.println("-------");

  if(Serial.available()) {
    char temp = Serial.read();
    if(temp == 'f') {
      findRightFactor();
    } else if (temp == 't') {
      scale.tare();
      scale2.tare();
    }
  } 
  
  delay(500);
}

void findRightFactor() {
  Serial.println("Start kalibracji...");
  bool isCorrect = false;
  
  while(isCorrect != true) {
    float readKg = scale.get_units(1);
    float readKg2 = scale2.get_units(1);

//    Serial.print("Belka lewa:\t");
//    Serial.print(readKg);
//    Serial.print("\tBelka prawa:\t");
//    Serial.print(readKg2);

    float average = (readKg + readKg2) / 2;
    
    if(average == tareValue) {
      
      Serial.print("Ustawiono! -->");
      Serial.print("Zapisana kalibracja: ");
      Serial.println(calibration_factor);
      
      isCorrect = true;
      delay(5000);
      break;
    }

    int diffValue = 30;

    if (average < tareValue) {
      calibration_factor = calibration_factor + diffValue;
    } else {
      calibration_factor = calibration_factor - diffValue;
    }
    
    
    scale.set_scale(calibration_factor);
    scale2.set_scale(calibration_factor);

    Serial.print("Waga: ");
    Serial.print(average);
    Serial.print(" Factor: ");
    Serial.println(calibration_factor);
    delay(50);
    
//    Serial.print("[Zmiana kalibracji] Aktualna kalibracja: ");
//    Serial.println(calibration_factor);
  }
}
