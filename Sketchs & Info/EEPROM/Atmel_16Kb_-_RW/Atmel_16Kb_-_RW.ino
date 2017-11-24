#include <extEEPROM.h>
#include <SPIFlash.h>
#include <SPI.h>
#include <EEPROM.h>
#include <Wire.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include "Arduino.h"

#define EEPROM_I2C_ADDRESS 0x50

const unsigned long MEM_SIZE = 16384L;

void setup() {
  Serial.begin(9600);
  Wire.begin();
  delay(1000);
}

void Borrar(){
  byte x = 0xFF;
  
  for(unsigned long i=0; i<MEM_SIZE; i++){
    Wire.begin();
    Wire.write(x);
    Serial.print(Wire.write(x), HEX);
    Wire.end();
     }  
  delay(1000); 
}

void Leer(){ 
  byte readVal = 0;
  
  for(unsigned long i=0; i<MEM_SIZE; i++) {  
  readVal = readAddress(i);
  Serial.print((readVal), HEX);      
  }
  delay(100);
}
byte readAddress(unsigned int address)
{
  byte rData = 0xFF;
  
  Wire.beginTransmission(EEPROM_I2C_ADDRESS);
  Wire.write((unsigned int)(address >> 8));
  Wire.write((unsigned int)(address & 0xFF));
  Wire.endTransmission();  

  Wire.requestFrom(EEPROM_I2C_ADDRESS, 1);  

  rData =  Wire.read();

  return rData;
}
void loop() {
    if (Serial.available() > 0) {
    char character = Serial.read();
    switch (character) {
       case 'c':
      {
        Borrar();
        break;
      }
       case 'r':
      {
        Leer();
        break;
      }
    } 
  }
   delay(100);
}
