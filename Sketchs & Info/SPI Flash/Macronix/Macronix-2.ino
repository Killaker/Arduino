// MX25L6405 SPI
// Datasheet http://www.macronix.com/QuickPlace/hq/PageLibrary4825740B00298A3B.nsf/$defaultview/3F21BAC2E121E17848257639003A3146/$File/MX25L6405,%203V,%2064Mb,%20v1.3.pdf?OpenElement
// Some code from http://arduino.cc/en/Tutorial/SPIEEPROM
#include <SPIFlash.h>
#include <SPI.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include "Arduino.h"

// Pin Definitions
#define DATAOUT 11 // MOSI
#define DATAIN 12 // MISO 
#define SPICLOCK 13 // sck
#define SLAVESELECT 10 // ss
#define MANUFACTURER_ID 0xC220
#define FLASH_SS 10

// Command Definitions
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
SPIFlash flash(FLASH_SS, MANUFACTURER_ID);


void setup()
{
  Serial.begin(115200);
  delay(2000);

  pinMode(FLASH_SS, OUTPUT);
  pinMode(DATAOUT     , OUTPUT);
  pinMode(DATAIN      , INPUT);
  pinMode(SPICLOCK    , OUTPUT);
  pinMode(SLAVESELECT , OUTPUT);
  
  // Data sheet says this must be high
  digitalWrite(SLAVESELECT, HIGH);

  Serial.print("Start...");

  if (flash.initialize())
    Serial.println("Init OK!");
  else
    Serial.println("Init FAIL!");
  
  // SPCR = 01010000
  //interrupt disabled, spi enabled, msb 1st, master, clk low when idle,
  //sample on leading edge of clk, system clock/4 rate (fastest)
  SPCR = (1 << SPE)|(1 << MSTR);
  clr = SPSR;
  clr = SPDR;

  delay(1000);
}

void ReadID() {
  digitalWrite(SLAVESELECT, LOW);
  spi_transfer(RDID);
  
  int b1 = spi_transfer(0xFF);
  int b2 = spi_transfer(0xFF);
  int b3 = spi_transfer(0xFF);

  digitalWrite(SLAVESELECT, HIGH);

  //Serial.write(b1);
  //Serial.write(b2);
  //Serial.write(b3);

  
  sprintf(fmt, "%02X %02X %02X\n", b1, b2, b3);
  Serial.print(fmt);
}

void ReadElectronicId() {
  digitalWrite(SLAVESELECT, LOW);
  spi_transfer(RES);
  
  spi_transfer(0x00);
  spi_transfer(0x00);
  spi_transfer(0x00);
  
  int b1 = spi_transfer(0xFF);
  
  digitalWrite(SLAVESELECT, HIGH);
  
  sprintf(fmt, "%02X\n", b1);
  Serial.print(fmt);
}

void ReadManufactureId() {
  digitalWrite(SLAVESELECT, LOW);
  spi_transfer(REMS);
  
  spi_transfer(0x00);
  spi_transfer(0x00);
  spi_transfer(0x00);
  
  int b1 = spi_transfer(0xFF);
  int b2 = spi_transfer(0xFF);
  int b3 = spi_transfer(0xFF);
  
  digitalWrite(SLAVESELECT, HIGH);
  
  sprintf(fmt, "%02X %02X %02X\n", b1, b2, b3);
  Serial.print(fmt);
}

void ReadStatusReg() {
  digitalWrite(SLAVESELECT, LOW);
  spi_transfer(RDSR);
  
  int b1 = spi_transfer(0xFF);
  
  digitalWrite(SLAVESELECT, HIGH);
  
  sprintf(fmt, "%02X\n", b1);
  Serial.print(fmt);
}

void Dump() {
  int data;
  digitalWrite(SLAVESELECT, LOW);
  spi_transfer(READ); // Address auto increments so only need to do a read instruction once
  
  // Address 0x000000
  spi_transfer(0x00);
  spi_transfer(0x00);
  spi_transfer(0x00);

  for (int i = 0, x = 0; i < 0x800000; i++, x++) {
    if (x >= 16) {
    //  Serial.print('\n');
    x = 0;
    } 
    sprintf(fmt, "%02X", spi_transfer(0xFF));
    Serial.print(fmt);
    //Serial.print(spi_transfer(0xFF), HEX);
   //Serial.write(spi_transfer(0xFF));
  }
    digitalWrite(SLAVESELECT, HIGH);
}
void loop()
{
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
      case 's':
      {
        ReadStatusReg();
        break;
      }
      case 'm':
      {
        ReadManufactureId();
        break;
      }
      case 'e':
      {
        ReadElectronicId();
        break;
      }
       default:
       {
         Serial.println("Unknown Command");
         break;
       }
    }
  }
  
  //ReadID(); // RDID
  //ReadElectronicId(); // RES
  //ReadManufactureId(); // REMS
  //Dump();
  
  //while (1) {
    delay(1000);
  //}
}
