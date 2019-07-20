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

//#define RS485_USI_SERIAL

#include <SoftwareSerial.h>
#ifdef RS485_USI_SERIAL
#include <UsiSerial.h>
#endif
#include <MAX485.h>

#define SOFT_SERIAL_USB

// Same pin for RE and DE 7->RE (MAX485 PIN 2), 7->DE (MAX485 PIN 3)
#define PIN_RS485_TE          1

// RX(10)->RO (MAX845 PIN 1), TX(11)->DI (MAX845 PIN 4)
#define PIN_SOFT_485_RX_RO    0
#define PIN_SOFT_485_TX_DI    2

#define PIN_SOFT_USB_RX       3
#define PIN_SOFT_USB_TX       4

#define BAUD_RATE          9600
#ifdef RS485_USI_SERIAL
#define BAUDRATE      BAUD_RATE
#endif

//#define WAIT_FOR_SERIAL
#define SEND_ALL_BUFFERED
#define BUFFER_SIZE         384

SoftwareSerial softser485(PIN_SOFT_485_RX_RO, PIN_SOFT_485_TX_DI);
#ifdef SOFT_SERIAL_USB
SoftwareSerial softserusb(PIN_SOFT_USB_RX, PIN_SOFT_USB_TX);
#endif

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


#ifdef SOFT_SERIAL_USB
uint16_t at, i;
uint8_t* buffer;
#else
uint8_t b;
#endif

void setup() {
#ifdef SOFT_SERIAL_USB
  buffer = (uint8_t*)(malloc(BUFFER_SIZE));
#endif

#ifdef PIN_INDICATOR_LED_DIRECTION_A
  indleda.initialize(0);
  indleda.fullcycle();
#endif
#ifdef PIN_INDICATOR_LED_DIRECTION_B
  indledb.initialize(0);
  indledb.fullcycle();
#endif

#ifdef RS485_USI_SERIAL
  UsiSerial.begin();
#else
#ifdef SOFT_SERIAL_USB
  softserusb.begin(BAUD_RATE);
#else
  Serial.begin(BAUD_RATE);
#endif
#ifdef WAIT_FOR_SERIAL
  while (!Serial) { ; /* wait for serial USB-port to connect */ }
#endif
#endif

  // set the data rate for the SoftwareSerial port connected to the TTL to RS-485 module
  softser485.begin(BAUD_RATE);

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

#ifdef SOFT_SERIAL_USB
void loop() {
  at = 0x0000;
  while (softserusb.available() && at < BUFFER_SIZE) {
#ifdef USING_MAX_485
    if (!issending) {
      max485.sending(issending = true);
    }
#endif
    buffer[at++] = softserusb.read();
  }
  if(at < BUFFER_SIZE - 1) {
    for(i = 0; i < at; i++) {
      softser485.write(buffer[i]);
    }
  }
#ifdef USING_MAX_485
  if (issending) {
    max485.sending(issending = false);
  }
#endif
  at = 0x0000;
  while(softser485.available() && at < BUFFER_SIZE) {
    buffer[at++] = softser485.read();
  }
  if(at < BUFFER_SIZE - 1) {
    for(i = 0; i < at; i++) {
      softserusb.write(buffer[i]);
    }
  }
}
#else
void loop() {
#ifdef SEND_ALL_BUFFERED
#ifdef RS485_USI_SERIAL
  while (UsiSerial.available()) {
#else
  while (Serial.available()) {
#endif
#else
#ifdef RS485_USI_SERIAL
  if (UsiSerial.available()) {
#else
  if (Serial.available()) {
#endif
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
#ifdef RS485_USI_SERIAL
    b = UsiSerial.read();
#else
    b = Serial.read();
#endif
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
#ifdef RS485_USI_SERIAL
    UsiSerial.write(b);
#else
    Serial.write(b);
#endif
  }
#ifdef PIN_INDICATOR_LED_DIRECTION_B
  if (isreceiving) {
    digitalWrite(PIN_INDICATOR_LED_DIRECTION_B, LOW);
    isreceiving = false;
  }
#endif
}
#endif
