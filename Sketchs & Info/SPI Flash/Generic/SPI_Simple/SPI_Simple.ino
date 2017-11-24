#include <EEPROM.h>
#include <SerialFlash.h>
#include <SomeSerial.h>
#include <SoftwareSerial.h>
#include <SPIFlash.h>
#include <SPI.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include "Arduino.h"

#define DATAOUT 11//MOSI
#define DATAIN  12//MISO 
#define SPICLOCK  13//sck
#define SLAVESELECT 10//ss
#define MANUFACTURER_ID 0xC220
#define FLASH_SS 10
#define SERIAL_BAUD 115200

//COMMANDS
#define WREN 0x06 // Sets the (WEL) write enable latch bit
#define WRDI 0x04 // Reset the (WEL) write enable latch bit
#define RDID 0x9F // Output the manufacturer ID and 2-byte device ID (outputs 3 bytes)
#define RDSR 0x05 // To read out the status register (outputs 1 byte)
#define WRSR 0x01 // To write new values to the status register
#define READ 0x03 // N bytes read out until CS# goes high (requires input address of 3 bytes) (outputs ? bytes)
#define FAST_READ 0x0B // (requires input address of 3 bytes, optional fourth?) (outputs ? bytes)
#define PARALLEL_MODE 0x55 // Enter and stay in parallel mode until power off
#define SE 0x20 // Sector erase (input 3 bytes address)
#define SE_ALT 0xD8 
#define CE 0x60 // Chip erase
#define CE_ALT 0xC7
#define PP 0x02 // Page Program (input 3 byte address)
#define DP 0xB9 // Deep Power Down
#define EN4K 0xB5 // Enter 4Kb sector
#define EX4K 0xB5 // Exit 4Kb sector
#define RDP 0xAB // Release from deep power down
#define RES 0xAB // Read electronic id (optional 3 byte input?)
#define REMS 0x90 // Read electronic manufacturer & device id (2 byte optional? third byte if 0 will 
                   // output manufacturer id first, 1 will output device id first)
char fmt[16]; // Some place to sprintf into
byte eeprom_output_data = 0;
byte eeprom_input_data = 0;
byte clr = 0;

byte spi_transfer(volatile byte data)
{
  SPDR = data;                    // Start the transmission
  while (!(SPSR & (1 << SPIF)))   // Wait the end of the transmission
  {
  };
  return SPDR;                    // return the received byte
}
char input = 0;
long lastPeriod = -1;

//////////////////////////////////////////
// #define flash(SPI_CS, MANUFACTURER_ID)
// SPI_CS - CS pin attached to SPI flash chip (8 in case of Moteino)
// MANUFACTURER_ID - OPTIONAL, 0x1F44 for adesto(ex atmel) 4mbit flash
// 0xEF30 for windbond 4mbit flash
//////////////////////////////////////////
SPIFlash flash(FLASH_SS, MANUFACTURER_ID);
long address = 0;
byte value;
byte myByte; 

void setup(){
  Serial.begin(SERIAL_BAUD);
  delay(1000);
  
  pinMode(FLASH_SS, OUTPUT);
  pinMode(DATAOUT, OUTPUT);
  pinMode(DATAIN, INPUT);
  pinMode(SPICLOCK,OUTPUT);
  pinMode(SLAVESELECT,OUTPUT);
  
  Serial.print("Start...");

  if (flash.initialize())
    Serial.println("Init OK!");
  else
    Serial.println("Init FAIL!");
      delay(1000);
}

void Dump(){
Serial.println("Flash content:"); 
     digitalWrite(SLAVESELECT, LOW);
       long address = 0;
       while (address<=8388608){
          // every time a byte is read it is expunged 
      // from the serial buffer so keep reading the buffer until all the bytes 
      // have been read.
        (myByte = flash.readByte(address++), HEX); // read in the next byte
      //Serial.print(myByte, DEC); // base 10, this is the default
      //Serial.print(myByte, HEX); // base 16
      //Serial.print(myByte, OCT); // base 8
      //Serial.print(myByte, BIN); // base 2
      Serial.write(myByte); // ASCII character
      // a short delay
      
   }
        flash.end();
        digitalWrite(SLAVESELECT, HIGH);
        delay(1000);
}



void Erase(){
      digitalWrite(SLAVESELECT, LOW);
      Serial.print("Erasing Flash chip ... ");
      flash.chipErase();
      while(flash.busy());
      Serial.println("DONE");
      digitalWrite(SLAVESELECT, HIGH);
        delay(1000);
    }  
void ReadID() {
      digitalWrite(SLAVESELECT, LOW);
      Serial.print("DeviceID: ");
      Serial.println(flash.readDeviceId(), HEX);
      digitalWrite(SLAVESELECT, HIGH);
        delay(1000);
    }
void loop(){
  if (Serial.available() > 0) {
    char character = Serial.read();
    
    switch (character) {
      case 'i':
      {
        ReadID();
        break;
      }
      case 'd':
      {
        Dump();
        break;
      }
      case 'e':
      {
        Erase();
        break;
      }
       default:
       {
         Serial.println("Unknown Command");
         break;
           delay(1000);
       }
    }
  }  delay(1000);
}

