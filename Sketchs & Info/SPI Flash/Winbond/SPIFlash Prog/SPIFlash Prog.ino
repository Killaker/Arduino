#include <SPIFlash.h>
#include <SPI.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include "Arduino.h"

//PINES
#define SLAVESELECT 10
#define DATAOUT 11 
#define MISO  12
#define SPICLOCK  13

//WINBOND W25Q32
#define MANUFACTURER_ID 0xEF40 

//COMANDOS
#define WREN 0x06 // Sets the (WEL) write enable latch bit
#define WRDI 0x04 // Reset the (WEL) write enable latch bit
#define RDID 0x90 // Output the manufacturer ID and 2-byte device ID (outputs 3 bytes)
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
#define REMS 0x9F // Read electronic manufacturer & device id (2 byte optional? third byte if 0 will 

SPIFlash flash(SLAVESELECT, MANUFACTURER_ID);

byte clr = 0;
char fmt[16];
long buffer[256];
int counter;
long counter2;
long address = 4194304L;

static const uint32_t PROGMEM crc_table[16] = {
    0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac,
    0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
    0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
    0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c
};
unsigned long crc_update(unsigned long crc, byte data){
    byte tbl_idx;
    tbl_idx = crc ^ (data >> (0 * 4));
    crc = pgm_read_dword_near(crc_table + (tbl_idx & 0x0f)) ^ (crc >> 4);
    tbl_idx = crc ^ (data >> (1 * 4));
    crc = pgm_read_dword_near(crc_table + (tbl_idx & 0x0f)) ^ (crc >> 4);
    return crc;
}
unsigned long crc_buffer(void){
  unsigned long crc = ~0L;
  for(int i=0; i < 256; i++)
    crc = crc_update(crc, buffer[i]);
  crc = ~crc;
  return crc;
}
void setup(){
  
  Serial.begin(57600);
  SPI.begin();
  
  delay(100);
  
  pinMode(MISO, INPUT_PULLUP);
  pinMode(SPICLOCK,OUTPUT);
  pinMode(DATAOUT, OUTPUT);
  pinMode(SLAVESELECT,OUTPUT);
  
  digitalWrite(SLAVESELECT, HIGH);
  
  Serial.println("Comprobando tipo de flash...");

  if (flash.initialize()){
      Serial.println("FLASH W25Q32 encontrada!");
      Serial.print("ID Dispositivo: 0x");
      Serial.println(flash.readDeviceId(), HEX);
  }
  else {
    Serial.println("ERROR! Configuracion eronea para esta Flash!");
  }
  SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPI2X);
  clr = SPSR;
  clr = SPDR;       
   delay(10);    
 }
void dump_buffer(){
  
  for(counter = 0; counter < 256; counter++) {
    Serial.print(buffer[counter] >> 4, HEX);
    Serial.print(buffer[counter] & 0xF, HEX);
  }
  Serial.println();
}
void dump_buffer_crc(){
  unsigned long crc = crc_buffer();
  Serial.print(crc_buffer(), HEX);
  Serial.println();
}

void load_buffer(){
  for(counter = 0; counter < 256; counter++) {
    buffer[counter] = read_hex();
  }
}
byte read_nybble(){
  int nybble;
  while((nybble = Serial.read()) == -1);

  if(nybble >= 'A') {
    return 9 + (nybble & 0x0f);
  }
   else { 
    return nybble & 0x0f;
  } 
}
byte read_hex(){
  byte val;
  
  val = (read_nybble() & 0xf) << 4;
  val |= read_nybble();
  
  return val;
}
byte spi_transfer(volatile char data) {
  
  SPDR = data;
  while (!(SPSR & (1 << SPIF)))
  {
  };
  return SPDR;
}
void read_page(byte adr1, byte adr2){
  int i;
  digitalWrite(SLAVESELECT,LOW);
  spi_transfer(READ);
  spi_transfer(adr1);
  spi_transfer(adr2);
  spi_transfer(0);
  
  for(counter = 0, i = 0; counter < 256; counter++, i++) {
     if (i >= 16) {
         i = 0;
    }
    buffer[counter] = spi_transfer(0xFF); 
  }
  digitalWrite(SLAVESELECT,HIGH);
} 
void wait_for_write(){
  byte statreg = 0x1;
  
  while((statreg & 0x1) == 0x1) {
    digitalWrite(SLAVESELECT, LOW);
    spi_transfer(RDSR);
    statreg = spi_transfer(RDSR);
    digitalWrite(SLAVESELECT, HIGH);
  }  
}
void write_page(byte adr1, byte adr2){

  digitalWrite(SLAVESELECT,LOW);
  spi_transfer(WREN);
  digitalWrite(SLAVESELECT,HIGH);
  delay(10);
  digitalWrite(SLAVESELECT,LOW);
  spi_transfer(PP); 
  spi_transfer(adr1); 
  spi_transfer(adr2); 
  spi_transfer(0); 
    
    for (counter = 0; counter < 256; counter++){
    spi_transfer(buffer[counter]);
  }
  digitalWrite(SLAVESELECT,HIGH);
  delay(1);  
}
void erase_all() {
    
  digitalWrite(SLAVESELECT,LOW);
  spi_transfer(WREN);
  digitalWrite(SLAVESELECT,HIGH);
  delay(10);

  digitalWrite(SLAVESELECT,LOW);
  spi_transfer(CE);
  Serial.print(spi_transfer(CE));
  digitalWrite(SLAVESELECT,HIGH);
  delay(1); 
  wait_for_write();
}
void erase_sector(byte addr1, byte addr2) {
  
  digitalWrite(SLAVESELECT,LOW);
  spi_transfer(WREN);
  digitalWrite(SLAVESELECT,HIGH);
  delay(10);
  
  digitalWrite(SLAVESELECT,LOW);
  spi_transfer(SE);
  spi_transfer(addr1);
  spi_transfer(addr2);
  spi_transfer(0);
  digitalWrite(SLAVESELECT,HIGH);
}
void Erase(){
     
     byte myByte2 = 0xFF;
  
     Serial.println("Borrando flash modo 0xFF : ");
     delay(800); 

  digitalWrite(SLAVESELECT,LOW);
  spi_transfer(WREN);
  digitalWrite(SLAVESELECT,HIGH);
  delay(10);
     
     digitalWrite(SLAVESELECT, LOW);
 
       for (counter2=0; counter2<address; counter2++){
        
       flash.writeByte(counter2, myByte2);
       Serial.print((myByte2), HEX); 
    } 
        Serial.println();
        Serial.println("Hecho!");
        digitalWrite(SLAVESELECT, HIGH);
        delay(100);
}
void Write() {

     byte myByte3 = 0xFB;
  
     Serial.println("Flasheando chip : ");

  digitalWrite(SLAVESELECT,LOW);
  spi_transfer(WREN);
  digitalWrite(SLAVESELECT,HIGH);
  delay(10);

      digitalWrite(SLAVESELECT,LOW);

       for (counter2=0; counter2<address; counter2++){
        
       flash.writeByte(counter2, myByte3);
       Serial.print((myByte3), HEX);       
    } 
        Serial.println();
        Serial.println("Hecho!");
        digitalWrite(SLAVESELECT, HIGH);       
        delay(100);
} 
void Dump(){

   int data;
    
   Serial.println("Dumpeando : "); 
   delay(800);
   digitalWrite(SLAVESELECT, LOW);

   spi_transfer(READ); 
   spi_transfer(0x00);
   spi_transfer(0x00);
   spi_transfer(0x00);

  for (int i = 0; i < 0x400000; i++) {

    sprintf(fmt, "%02X", spi_transfer(0xFF));
    Serial.print(fmt);
    }
        Serial.println("HECHO!");
        digitalWrite(SLAVESELECT, HIGH);       
        delay(100);
}
void ReadElectronicId() {
  digitalWrite(SLAVESELECT, LOW);
  spi_transfer(RES);  
  spi_transfer(0x00);
  spi_transfer(0x00);
  spi_transfer(0x00);
  
   Serial.print("Electronic ID: ");
   Serial.println(spi_transfer(0xFF), HEX);
        digitalWrite(SLAVESELECT, HIGH);
        delay(100);
}
void ReadStatusReg() {
  digitalWrite(SLAVESELECT, LOW);
  spi_transfer(RDSR);  
  spi_transfer(0xFF);  
  Serial.print("Status Register: ");
  Serial.println(spi_transfer(0xFF), HEX);
  digitalWrite(SLAVESELECT, HIGH);          
  delay(100);
}

void ReadManufactureId() {
  digitalWrite(SLAVESELECT, LOW);
  spi_transfer(RDID);  
  spi_transfer(0x00);
  spi_transfer(0x00);
  spi_transfer(0x00);
  
  int b1 = spi_transfer(0xFF);
  int b2 = spi_transfer(0xFF);

  Serial.print("Manufacturer ID: ");
  Serial.print((b1), HEX);
  Serial.print((b2), HEX);
  Serial.println();
     digitalWrite(SLAVESELECT, HIGH);
          delay(100);
} 
void ReadID() {
      digitalWrite(SLAVESELECT, LOW);
      Serial.print("Device ID: ");
      Serial.println(flash.readDeviceId(), HEX);
      digitalWrite(SLAVESELECT, HIGH);
       delay(100);
}
void loop(){

  byte addr1, addr2;
  
  while (Serial.available() == 0);
    
   int cmd = Serial.read();
    switch (cmd) {
      case 'S':
      {
       ReadStatusReg();
        break;
      }
      case 'M':
      {
       ReadManufactureId();
        break;
      }
      case 'E':
      {
       ReadElectronicId();
        break;
      }
      case 'I':
      {
       ReadID();
        break;
      }
      case 'D':
      {
       Dump();
        break;
      }
      case 'X':
      {
       Erase();
        break;
      }
      case 'W':
      {
       Write();
        break;
      }
      case '>':   
       Serial.print('>');
        break;
      
      case 'e':
       erase_all();
        break;
  
      case 'r':
       addr1 = read_hex();
       addr2 = read_hex();
       read_page(addr1, addr2);
        break;
  
      case 'w':
       addr1 = read_hex();
       addr2 = read_hex();
       write_page(addr1, addr2);
        break;
  
      case 'd':
       dump_buffer();
        break;
  
      case 'c':
       dump_buffer_crc();
        break;
  
      case 'l':
       load_buffer();
        break;
  
      case 's':
       addr1 = read_hex();
       addr2 = read_nybble() << 4;
       erase_sector(addr1, addr2);
        break;
  
      case '?':
      case 'h':
       Serial.println("\nSPI Flash programmer");
       Serial.println(" e    : erase chip");
       Serial.println(" sXXX : erase 4k sector XXX (hex)");
       Serial.println(" c    : print buffer CRC-32");
       Serial.println(" rXXXX: read 256-byte page XXXX (hex) to buffer");
       Serial.println(" wXXXX: write buffer to 256-byte page XXXX (hex)");
       Serial.println(" d    : display the buffer (in hex)");
       Serial.println(" l<b> : load the buffer with <b>, where b is 256 bytes of data");
       Serial.println();
       Serial.println("Ex: r3700 - read 256 bytes from page 0x3700");
       Serial.println("Ex: lcafe[...]3737 - load the buffer with 256 bytes, first byte 0xca...");
 }    
//  Serial.flush(); 
}
//Erase- x
//Write- w
//Dump- d
//ReadID- i
//ReadElectronicId- e
//ReadManufacturerId- m
//ReadStatusReg- s
