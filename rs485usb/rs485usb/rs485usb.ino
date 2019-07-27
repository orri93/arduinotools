/*
 * Name:		rs485usb.ino
 * Created:	7/14/2019 2:23:09 AM
 * Author:	SigurdssonGO
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

#include <SoftwareSerial.h>
#include <MAX485.h>

#include "pinuno.h"

#define BAUD_RATE     9600

#define SEND_ALL_BUFFERED

//#define WAIT_FOR_SERIAL

SoftwareSerial softser(PIN_SOFT_RX_RO, PIN_SOFT_TX_DI);

#if (defined(PIN_RS485_RE) && defined(PIN_RS485_DE)) || defined(PIN_RS485_TE)
#define USING_MAX_485
#define RS485_MASTER
// Setup max485 Control Input
#ifdef PIN_RS485_TE
MAX485 max485(PIN_RS485_TE);
#else
MAX485 max485(PIN_RS485_RE, PIN_RS485_DE);
#endif
bool issending = false, isreceiving = false;
#endif


#if defined(PIN_INDICATOR_LED_DIRECTION_A) || defined(PIN_INDICATOR_LED_DIRECTION_B)
#include <arduinosinled.h>
#endif


#ifdef PIN_INDICATOR_LED_DIRECTION_A
fds::SinLed indleda(PIN_INDICATOR_LED_DIRECTION_A);
#endif


#ifdef PIN_INDICATOR_LED_DIRECTION_B
fds::SinLed indledb(PIN_INDICATOR_LED_DIRECTION_B);
#endif

uint8_t b;

void setup() {

#ifdef PIN_INDICATOR_LED_DIRECTION_A
  indleda.initialize(0);
  indleda.fullcycle();
#endif

#ifdef PIN_INDICATOR_LED_DIRECTION_B
  indledb.initialize(0);
  indledb.fullcycle();
#endif

  Serial.begin(BAUD_RATE);
#ifdef WAIT_FOR_SERIAL
  while (!Serial) { ; /* wait for serial USB-port to connect */ }
#endif

  // set the data rate for the SoftwareSerial port connected to the TTL to RS-485 module
  softser.begin(BAUD_RATE);

#ifdef USING_MAX_485
#ifdef RS485_MASTER
  max485.initialize(Rs485Role::Master);
#else
  // set Slave-Mode (not really necessary since this is the default library configuration)
  max485.initialize(Rs485Role::Slave);
#endif
  max485.sending(issending);
#endif
}

void loop() {
#ifdef SEND_ALL_BUFFERED
  while (Serial.available()) {
#else
  if (Serial.available()) {
#endif
#ifdef USING_MAX_485
    if (!issending) {
#ifdef PIN_INDICATOR_LED_DIRECTION_A
      digitalWrite(PIN_INDICATOR_LED_DIRECTION_A, HIGH);
#endif
      issending = true;
      max485.sending(issending);
    }
#endif
    b = Serial.read();
    softser.write(b);
  }
#ifdef USING_MAX_485
  if (issending) {
#ifdef PIN_INDICATOR_LED_DIRECTION_A
    digitalWrite(PIN_INDICATOR_LED_DIRECTION_A, LOW);
#endif
    issending = false;
    max485.sending(issending);
  }
#endif
#ifdef SEND_ALL_BUFFERED
  while (softser.available()) {
#else
  if (altserial.available()) {
#endif
#ifdef PIN_INDICATOR_LED_DIRECTION_B
    if (!isreceiving) {
      digitalWrite(PIN_INDICATOR_LED_DIRECTION_B, HIGH);
      isreceiving = true;
    }
#endif
    b = softser.read();
    Serial.write(b);
  }
#ifdef PIN_INDICATOR_LED_DIRECTION_B
  if (isreceiving) {
    digitalWrite(PIN_INDICATOR_LED_DIRECTION_B, LOW);
    isreceiving = false;
  }
#endif
}
