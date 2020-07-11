#include "HX711.h"

#define DOUT2 5
#define CLK2 4
#define DOUT  3
#define CLK  2

HX711 scale;
HX711 scale2;

float tareValue = 0.5;
float calibration_factor = -32150;
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
    if(temp == 'find') {
      findRightFactor();
    } else if (temp == 'tare') {
      scale.tare();
      scale2.tare();
    }
  } 
  
  delay(500);
}
