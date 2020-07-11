#include "HX711.h"

#define DOUT  3
#define CLK  2

HX711 scale;

float tareValue = 0.5;
float calibration_factor = 32150;
//float calibration_factor = -10000;//-7050 worked for my 440lb max scale setup

void setup() {
  Serial.begin(9600);
  
  scale.begin(DOUT, CLK);
  scale.set_scale();
  scale.tare();
  
  
}

void loop() {
  
  scale.set_scale(calibration_factor);
  
  float readKg = scale.get_units(4);
  
  Serial.println(readKg);

  if(Serial.available()) {
    char temp = Serial.read();
    if(temp == '+') {
      findRightFactor();
    } else if (temp == 't') {
      scale.tare();
    }
  } 
}

void findRightFactor() {
  
  bool nieUstawiono = true;
  
  while(nieUstawiono = true) {
    float readKg = scale.get_units(1);

    Serial.print("Waga: ");
    Serial.print(readKg);
    
    if(readKg > tareValue - 0.01 && readKg < tareValue + 0.01) {
      Serial.println("Ustawiono!");
      nieUstawiono = false;
      break;
    }

    calibration_factor = calibration_factor + 20;
    scale.set_scale(calibration_factor);
    Serial.print(" // Zmiana kalibracji: ");
    Serial.println(calibration_factor);
  }
}
