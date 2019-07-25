  /*
 * Name:    rs485usb.ino
 * Created: 7/14/2019 2:23:09 AM
 * Author:  SigurdssonGO
 * 
 * Software Serial Limitations
 * 
 * The RX pin must support change interrupt
 * On the Mega and Mega 2560 support the following can be used for RX: 10, 11, 12, 13, 14, 15, 50, 51, 52, 53, A8 (62), A9 (63), A10 (64), A11 (65), A12 (66), A13 (67), A14 (68), A15 (69).
 * On the Leonardo and Micro the following can be used for RX: 8, 9, 10, 11, 14 (MISO), 15 (SCK), 16 (MOSI)
 * 
 * On Arduino or Genuino 101 the current maximum RX speed is 57600bps
 * On Arduino or Genuino 101 RX doesn't work on Pin 13
 * 
 */

#include <MAX485.h>

#define BAUD_RATE     9600

#define PIN_RS485_TE     4

#ifdef PIN_RS485_TE
MAX485 max485(PIN_RS485_TE);
#endif

uint16_t b;
bool issending = false, isreceiving = false;

void setup() {

  Serial.begin(BAUD_RATE);

  // set the data rate for the SoftwareSerial port connected to the TTL to RS-485 module
  Serial1.begin(BAUD_RATE);

  max485.initialize(Rs485Role::Master);
  max485.sending(issending);
}

void loop() {

  if(Serial && Serial1) {
    while (Serial.available()) {
    if (!issending) {
      issending = true;
      max485.sending(issending);
    }
    b = Serial.read();
    Serial1.write(b);
  }
  if (issending) {
    issending = false;
    max485.sending(issending);
  }
  while (Serial1.available()) {
    b = Serial1.read();
    Serial.write(b);
  }
}
