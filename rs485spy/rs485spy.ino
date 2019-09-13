/*
 * Name:    rs485spy.ino
 * Created: 12.09.2019
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

#include "option.h"

//#define OPTION_PIN          OPTION_PRO_MICRO
#define OPTION_PIN          OPTION_UNO

#define OPTION_USB_SERIAL   OPTION_HARDWARE_SERIAL
//#define OPTION_485_SERIAL   OPTION_HARDWARE_SERIAL_1
#define OPTION_485_SERIAL   OPTION_SOFTWARE_SERIAL

//#define OPTION_WAIT_SERIAL

#if (OPTION_USB_SERIAL == OPTION_SOFTWARE_SERIAL) || (OPTION_485_SERIAL == OPTION_SOFTWARE_SERIAL)
#include <SoftwareSerial.h>
#endif
#if (OPTION_USB_SERIAL == OPTION_SOFTWARE_SERIAL_ALT) || (OPTION_485_SERIAL == OPTION_SOFTWARE_SERIAL_ALT)
#include <AltSoftSerial.h>
#endif

#if OPTION_PIN == OPTION_UNO
#include "pinuno.h"
#elif OPTION_PIN == OPTION_PRO_MICRO
#include "pinpromicro.h"
#endif

#define BAUD_RATE     9600

#if     OPTION_USB_SERIAL == OPTION_HARDWARE_SERIAL
#define BEGIN_USB(x) Serial.begin(x)
#define WAIT_USB(...) while (!Serial){;/* Wait for serial port to connect */}
Stream& streamUsb = Serial;
#elif   OPTION_USB_SERIAL == OPTION_HARDWARE_SERIAL_1
#define BEGIN_USB(x) Serial1.begin(x)
#define WAIT_USB(...) while (!Serial1){;/* Wait for serial port to connect */}
Stream& streamUsb = Serial1;
#elif   OPTION_USB_SERIAL == OPTION_SOFTWARE_SERIAL
#define BEGIN_USB(x) serialusb.begin(x)
#define WAIT_USB(...) {;/* Do nothing */}
SoftwareSerial serialusb(0, 1);
Stream& streamUsb = serialusb;
#else
#endif

#if     OPTION_485_SERIAL == OPTION_HARDWARE_SERIAL
#define BEGIN_485(x) Serial.begin(x)
#define WAIT_485(...) while (!Serial){;/* Wait for serial port to connect */}
Stream& stream485 = Serial;
#elif   OPTION_485_SERIAL == OPTION_HARDWARE_SERIAL_1
#define BEGIN_485(x) Serial1.begin(x)
#define WAIT_485(...) while (!Serial1){;/* Wait for serial port to connect */}
Stream& stream485 = Serial1;
#elif   OPTION_485_SERIAL == OPTION_SOFTWARE_SERIAL
#define BEGIN_485(x) serial485.begin(x)
#define WAIT_485(...)  {;/* Do nothing */}
SoftwareSerial serial485(PIN_SOFT_RX_RO, PIN_SOFT_TX_DI);
Stream& stream485 = serial485;
#else
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

  BEGIN_USB(BAUD_RATE);
  BEGIN_485(BAUD_RATE);

#ifdef OPTION_WAIT_SERIAL
  WAIT_USB();
  WAIT_485();
#endif
}

void loop() {
  while (stream485.available()) {
#ifdef PIN_INDICATOR_LED_DIRECTION_
    digitalWrite(PIN_INDICATOR_LED_DIRECTION_A, HIGH);
#endif
  }
#ifdef SEND_ALL_BUFFERED
  while (stream485.available()) {
#else
  if (stream485.available()) {
#endif
#ifdef PIN_INDICATOR_LED_DIRECTION_B
    if (!isreceiving) {
      digitalWrite(PIN_INDICATOR_LED_DIRECTION_B, HIGH);
      isreceiving = true;
    }
#endif
    b = stream485.read();
    streamUsb.write(b);
  }
#ifdef PIN_INDICATOR_LED_DIRECTION_B
  if (isreceiving) {
    digitalWrite(PIN_INDICATOR_LED_DIRECTION_B, LOW);
    isreceiving = false;
  }
#endif

}
