/*
 * Name:		rs485usb2modbus.ino
 * Created:	7/14/2019 2:23:09 AM
 * Author:	SigurdssonGO
 *
 * Software Serial Limitations
 *
 * The RX pin must support change interrupt
 * On the Mega and Mega 2560 support the following can be used for RX:
 *   10, 11, 12, 13, 14, 15, 50, 51, 52, 53,
 *   A8 (62), A9 (63), A10 (64), A11 (65),
 *   A12 (66), A13 (67), A14 (68), A15 (69).
 * On the Leonardo and Micro the following can be used for RX:
 *   8, 9, 10, 11, 14 (MISO), 15 (SCK), 16 (MOSI)
 *
 * On Arduino or Genuino 101 the current maximum RX speed is 57600bps
 * On Arduino or Genuino 101 RX doesn't work on Pin 13
 *
 */

#include <SoftwareSerial.h>

#include <gatlled.h>

#include "rs485io.h"
#include "display.h"

#define BAUD_RATE     9600
#define USE_DISPLAY

/* ****************************************************************************
 * Arduino UNO pins
 *
 */
/* Same pin for RE and DE 8->RE (MAX485 PIN 2), 8->DE (MAX485 PIN 3)          */
#define PIN_RS485_TE     8
/* RX(10)->RO (MAX845 PIN 1), TX(11)->DI (MAX845 PIN 4)                       */
#define PIN_SOFT_RX_RO  10
#define PIN_SOFT_TX_DI   9
/* ****************************************************************************/

/* ****************************************************************************
 * Arduino Leonardo and Micro (ProMicro)
 *
 */
/* Same pin for RE and DE 7->RE (MAX485 PIN 2), 7->DE (MAX485 PIN 3)          */
// #define PIN_RS485_TE     7
/* Not all pins on the Leonardo and Micro support change interrupts,
 * so only the following can be used for RX:
 * 8, 9, 10, 11, 14 (MISO), 15 (SCK), 16 (MOSI).
 * RX(10)->RO (MAX845 PIN 1), TX(11)->DI (MAX845 PIN 4)                       */
// #define PIN_SOFT_RX_RO   13
// #define PIN_SOFT_TX_DI   5
/* ****************************************************************************/

#define PIN_LED_RS485_READ  5
#define PIN_LED_RS485_WRITE 6
#define PIN_LED_ERROR       7

#define SHOW_ERROR_TIME  1000

#define BUFFER_SIZE       255

#define INITIALIZE_DELAY  250

#define SerialUsb Serial

namespace gatl = ::gos::atl;
namespace  grm = ::gos::rs485::modbus;
namespace grmr = ::gos::rs485::modbus::read;

SoftwareSerial Serial485(PIN_SOFT_RX_RO, PIN_SOFT_TX_DI);

uint8_t buffer[BUFFER_SIZE];

unsigned long tick, clear_error = 0;

int request, response, available;

bool error = false;

void setup() {
#ifdef USE_DISPLAY
  grm::display::start();
#endif

#ifdef PIN_LED_RS485_READ
  gatl::led::initialize(PIN_LED_RS485_READ);
  gatl::led::blink(PIN_LED_RS485_READ);
  digitalWrite(PIN_LED_RS485_READ, LOW);
#endif

#ifdef PIN_LED_RS485_WRITE
  gatl::led::initialize(PIN_LED_RS485_WRITE);
  gatl::led::blink(PIN_LED_RS485_WRITE);
  digitalWrite(PIN_LED_RS485_WRITE, LOW);
#endif

#ifdef PIN_LED_ERROR
  gatl::led::initialize(PIN_LED_ERROR);
  gatl::led::blink(PIN_LED_ERROR);
  digitalWrite(PIN_LED_ERROR, LOW);
#endif

  SerialUsb.begin(BAUD_RATE);
  Serial485.begin(BAUD_RATE);

#ifdef PIN_RS485_TE
  pinMode(PIN_RS485_TE, OUTPUT);
#endif

#ifdef INITIALIZE_DELAY
  delay(INITIALIZE_DELAY);
#endif
}

void loop() {
  tick = millis();

  request = grmr::request(SerialUsb, buffer, BUFFER_SIZE);
  if (request > 0) {
#ifdef PIN_LED_RS485_READ
    digitalWrite(PIN_LED_RS485_READ, HIGH);
#endif

#ifdef PIN_RS485_TE
    /* Set DE and *RE to High for writing to RS485 */
    digitalWrite(PIN_RS485_TE, HIGH);
#endif

    /* Write the request to RS485 */
    Serial485.write(buffer, request);

#ifdef PIN_RS485_TE
    /* Set DE and *RE to Low for reading from RS485 */
    digitalWrite(PIN_RS485_TE, LOW);
#endif

#ifdef PIN_LED_RS485_READ
    digitalWrite(PIN_LED_RS485_READ, LOW);
#endif
    response = grmr::response(Serial485, buffer, BUFFER_SIZE);

    if (response > 0) {
#ifdef PIN_LED_RS485_WRITE
      digitalWrite(PIN_LED_RS485_WRITE, HIGH);
#endif
      /* Read the result to RS485 */
      SerialUsb.write(buffer, response);

#ifdef PIN_LED_RS485_WRITE
      digitalWrite(PIN_LED_RS485_WRITE, LOW);
#endif
    } else if (response < 0) {
#ifdef PIN_LED_ERROR
      digitalWrite(PIN_LED_ERROR, HIGH);
      clear_error = tick + SHOW_ERROR_TIME;
      error = true;
#endif
      while ((available = Serial485.available()) > 0) {
        Serial485.readBytes(buffer, min(available, BUFFER_SIZE));
      }
    }
  } else if (request < 0) {
#ifdef PIN_LED_ERROR
    digitalWrite(PIN_LED_ERROR, HIGH);
    clear_error = tick + SHOW_ERROR_TIME;
    error = true;
#endif
    while ((available = SerialUsb.available()) > 0) {
      SerialUsb.readBytes(buffer, min(available, BUFFER_SIZE));
    }
  }
  
#ifdef USE_DISPLAY
  grm::display::show(request, response);
#endif

#ifdef PIN_LED_ERROR
  if (error && tick >= clear_error) {
    digitalWrite(PIN_LED_ERROR, LOW);
    error = false;
  }
#endif

}
