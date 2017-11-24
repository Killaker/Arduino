/*
 * Renames the HM 10 bluetooth module
 */

#include <SoftwareSerial.h>
#include <ArduinoCommander.h>


// PINS
const int BLUETOOTH_TX = 8;
const int BLUETOOTH_RX = 7;

SoftwareSerial bluetooth(BLUETOOTH_TX, BLUETOOTH_RX);
ArduinoCommander phone(bluetooth); // pass reference of bluetooth object to ArduinoCommander.

void setup() {

    // Start bluetooth serial at 9600 bps
    bluetooth.begin(9600);

    delay(1000);

    // rename device to xxxxxxxx
    bluetooth.write("AT+NAMESnapBot");

}

void loop() {

}
