/*
 * ********************************************************
 * Mudbus Slave V0.0.1
 * ********************************************************
 * 
 * Slave id 1
 * 
 * Coils (0/1 values)
 *   00001 0x0000 Coil #1
 *   00002 0x0001 Coil #2
 * 
 * Discrete Input
 *   10001 0x0000 D3
 *   10002 0x0001 D4
 * 
 * Holding Registry
 *   40001 0x0000 Holding Registry #1
 *   40002 0x0001 Holding Registry #2
 * 
 * Input Registry
 *   30003 0x0002 A0
 *   30004 0x0003 A1
 * 
 * 
 * ********************************************************/

#include <ModbusSlave.h>

#include <arduinotick.h>
#include <arduinoformat.h>
#include <arduinodisplay.h>
#include <arduinosinled.h>

//#define _ARDUINO_SERIAL_DEBUG_BAUD_ 115200
#include <serialdebug.h>

#include "pinuno.h"
#include "fds-modbus-test-slave-logo.h"

#define MODBUS_SLAVE_ID         1
#define RS485_BAUD           9600

#define INTERVAL_DISPLAY      100
#define DURATION_WRITE_MSG   3000

#define BUFFER_SIZE            16

#ifdef PIN_LED_MODBUS_READ
fds::SinLed ledmodbusread(PIN_LED_MODBUS_READ);
#endif
#ifdef PIN_LED_MODBUS_WRITE
fds::SinLed ledmodbuswrite(PIN_LED_MODBUS_WRITE);
#endif
#ifdef PIN_COIL_LED_1
fds::SinLed ledcoil1(PIN_COIL_LED_1);
#endif
#ifdef PIN_COIL_LED_2
fds::SinLed ledcoil2(PIN_COIL_LED_2);
#endif
#ifdef PIN_HOLDING_REGISTRY_1
fds::SinLed ledholdingregistry1(PIN_HOLDING_REGISTRY_1);
#endif
#ifdef PIN_HOLDING_REGISTRY_2
fds::SinLed ledholdingregistry2(PIN_HOLDING_REGISTRY_2);
#endif

namespace fds {
namespace modbus {

enum Coil {
  Coil1 = 0x0000,
  Coil2 = 0x0001
};

enum DiscreteInput {
  DiscreteInputs1 = 0x0000,
  DiscreteInputs2 = 0x0001
};

enum HoldingRegistry {
  HoldingRegistry1 = 0x0000,
  HoldingRegistry2 = 0x0001
};

enum InputRegistry {
  InputRegistry1 = 0x0000,
  InputRegistry2 = 0x0001
};

static uint16_t address, i;

bool coil1 = false, coil2 = true;
uint16_t holdingregistry1 = 0x00, holdingregistry2 = 0xff;

static uint8_t result;

char writemsg1[BUFFER_SIZE];
char writemsg2[BUFFER_SIZE];

Modbus slave(MODBUS_SLAVE_ID, PIN_RS485_MODBUS_TE);

/* 0x01 Read Coils */
uint8_t read_coils(uint8_t fc, uint16_t startaddress, uint16_t length) {
#ifdef PIN_LED_MODBUS_READ
  digitalWrite(PIN_LED_MODBUS_READ, HIGH);
#endif
  result = STATUS_OK;
  for(i = 0; i < length; i++) {
    address = startaddress + i;
    switch(address) {
    case Coil1:
      slave.writeCoilToBuffer(i, coil1);
      break;
    case Coil2:
      slave.writeCoilToBuffer(i, coil2);
      break;
    default:
      result = STATUS_ILLEGAL_DATA_ADDRESS;
      break;
    }
  }
#ifdef PIN_LED_MODBUS_READ
  digitalWrite(PIN_LED_MODBUS_READ, LOW);
#endif
  return result;
}

/* 0x02 Read Discrete Inputs */
uint8_t read_discrete_inputs(uint8_t fc, uint16_t startaddress, uint16_t length) {
#ifdef PIN_LED_MODBUS_READ
  digitalWrite(PIN_LED_MODBUS_READ, HIGH);
#endif
  result = STATUS_OK;
  for(i = 0; i < length; i++) {
    address = startaddress + i;
    switch(address) {
    case DiscreteInputs1:
      slave.writeCoilToBuffer(i, digitalRead(PIN_DISCRETE_INPUTS_1));
      break;
    case DiscreteInputs2:
      slave.writeCoilToBuffer(i, digitalRead(PIN_DISCRETE_INPUTS_2));
      break;
    default:
      result = STATUS_ILLEGAL_DATA_ADDRESS;
      break;
    }
  }
#ifdef PIN_LED_MODBUS_READ
  digitalWrite(PIN_LED_MODBUS_READ, LOW);
#endif
  return result;
}

/* 0x03 Read Multiple Holding Registers */
uint8_t read_holding_registers(uint8_t fc, uint16_t startaddress, uint16_t length) {
#ifdef PIN_LED_MODBUS_READ
  digitalWrite(PIN_LED_MODBUS_READ, HIGH);
#endif
  result = STATUS_OK;
  for(i = 0; i < length; i++) {
    address = startaddress + i;
    switch(address) {
    case HoldingRegistry1:
      slave.writeRegisterToBuffer(i, holdingregistry1);
      break;
    case HoldingRegistry2:
      slave.writeRegisterToBuffer(i, holdingregistry2);
      break;
    default:
      result = STATUS_ILLEGAL_DATA_ADDRESS;
      break;
    }
  }
#ifdef PIN_LED_MODBUS_READ
  digitalWrite(PIN_LED_MODBUS_READ, LOW);
#endif
  return result;
}

/* 0x04 Read Input Registers */
uint8_t read_input_registers(uint8_t fc, uint16_t startaddress, uint16_t length) {
#ifdef PIN_LED_MODBUS_READ
  digitalWrite(PIN_LED_MODBUS_READ, HIGH);
#endif
  result = STATUS_OK;
  for(i = 0; i < length; i++) {
    address = startaddress + i;
    switch(address) {
    case InputRegistry1:
      slave.writeRegisterToBuffer(i, analogRead(PIN_INPUT_REGISTRY_1));
      break;
    case InputRegistry2:
      slave.writeRegisterToBuffer(i, analogRead(PIN_INPUT_REGISTRY_2));
      break;
    default:
      result = STATUS_ILLEGAL_DATA_ADDRESS;
      break;
    }
  }
#ifdef PIN_LED_MODBUS_READ
  digitalWrite(PIN_LED_MODBUS_READ, LOW);
#endif
  return result;
}

/* 0x05 Write Single Coil and 0x0f Write Multiple Coils */
uint8_t write_coils(uint8_t fc, uint16_t startaddress, uint16_t length) {
#ifdef PIN_LED_MODBUS_WRITE
  digitalWrite(PIN_LED_MODBUS_WRITE, HIGH);
#endif
  result = STATUS_OK;
  for(i = 0; i < length; i++) {
    address = startaddress + i;
    switch(address) {
    case Coil1:
      coil1 = slave.readCoilFromBuffer(i);
      sprintf(writemsg1, coil1 ? "W HIGH to C 1" : "W LOW to C 1");
#ifdef PIN_COIL_LED_1
      if(!coil1) {
        digitalWrite(PIN_COIL_LED_1, LOW);
      }
#endif
      break;
    case Coil2:
      coil2 = slave.readCoilFromBuffer(i);
      sprintf(writemsg2, coil2 ? "W HIGH to C 2" : "W LOW to C 2");
#ifdef PIN_COIL_LED_2
      if(!coil2) {
        digitalWrite(PIN_COIL_LED_2, LOW);
      }
#endif
      break;
    default:
      result = STATUS_ILLEGAL_DATA_ADDRESS;
      break;
    }
  }
#ifdef PIN_LED_MODBUS_WRITE
  digitalWrite(PIN_LED_MODBUS_WRITE, LOW);
#endif
  return result;
}

/* 0x06 Write Single Holding Register and 0x10 Write Multiple Holding Registers */
uint8_t write_holding_registers(uint8_t fc, uint16_t startaddress, uint16_t length) {
#ifdef PIN_LED_MODBUS_WRITE
  digitalWrite(PIN_LED_MODBUS_WRITE, HIGH);
#endif
  result = STATUS_OK;
  for(i = 0; i < length; i++) {
    address = startaddress + i;
    switch(address) {
    case HoldingRegistry1:
      holdingregistry1 = slave.readRegisterFromBuffer(i);
      sprintf(writemsg1, "W %d to HR 1", holdingregistry1);
#ifdef PIN_HOLDING_REGISTRY_1
      if(holdingregistry1 >= 0 && holdingregistry1 <= 255) {
        analogWrite(PIN_HOLDING_REGISTRY_1, holdingregistry1);
      }
#endif
      break;
    case HoldingRegistry2:
      holdingregistry2 = slave.readRegisterFromBuffer(i);
      sprintf(writemsg2, "W %d to HR 2", holdingregistry2);
#ifdef PIN_HOLDING_REGISTRY_2
      if(holdingregistry2 >= 0 && holdingregistry2 <= 255) {
        analogWrite(PIN_HOLDING_REGISTRY_2, holdingregistry2);
      }
#endif
      break;
    default:
      result = STATUS_ILLEGAL_DATA_ADDRESS;
      break;
    }
  }
#ifdef PIN_LED_MODBUS_WRITE
  digitalWrite(PIN_LED_MODBUS_WRITE, LOW);
#endif
  return result;
}

}
}

Tick tickdisplay(INTERVAL_DISPLAY);

unsigned long tick, write_msg_lifetime = 0;

char* pointer;

void setup() {
  *fds::modbus::writemsg1 = '\0';
  *fds::modbus::writemsg2 = '\0';

  ARDUINO_SERIAL_DEBUG_SETUP();

  fds::format::setup();

  fds::display::u8g2.begin();
  fds::display::logo(
    fds_modbus_test_slave_logo_width,
    fds_modbus_test_slave_logo_height,
    fds_modbus_test_slave_logo_bits);

#ifdef PIN_HOLDING_REGISTRY_1
  ledholdingregistry1.initialize();
#endif
#ifdef PIN_HOLDING_REGISTRY_2
  ledholdingregistry2.initialize();
#endif
#ifdef PIN_LED_MODBUS_READ
  ledmodbusread.initialize();
#endif
#ifdef PIN_LED_MODBUS_WRITE
  ledmodbuswrite.initialize();
#endif
#ifdef PIN_COIL_LED_1
  ledcoil1.initialize();
#endif
#ifdef PIN_COIL_LED_2
  ledcoil2.initialize();
#endif
#ifdef DELAY_LOGO
  delay(DELAY_LOGO);
#endif

#ifdef PIN_HOLDING_REGISTRY_1
  if(fds::modbus::holdingregistry1 >= 0 && fds::modbus::holdingregistry1 <= 255) {
    analogWrite(PIN_HOLDING_REGISTRY_1, fds::modbus::holdingregistry1);
  }
#endif
#ifdef PIN_HOLDING_REGISTRY_2
  if(fds::modbus::holdingregistry2 >= 0 && fds::modbus::holdingregistry2 <= 255) {
    analogWrite(PIN_HOLDING_REGISTRY_2, fds::modbus::holdingregistry2);
  }
#endif


  // RS485 control pin must be output
  pinMode(PIN_RS485_MODBUS_TE, OUTPUT);

  pinMode(PIN_DISCRETE_INPUTS_1, INPUT);
  pinMode(PIN_DISCRETE_INPUTS_2, INPUT);

  fds::modbus::slave.cbVector[CB_READ_COILS] = fds::modbus::read_coils;
  fds::modbus::slave.cbVector[CB_READ_DISCRETE_INPUTS] = fds::modbus::read_discrete_inputs;
  fds::modbus::slave.cbVector[CB_READ_INPUT_REGISTERS] = fds::modbus::read_input_registers;
  fds::modbus::slave.cbVector[CB_READ_HOLDING_REGISTERS] = fds::modbus::read_holding_registers;
  fds::modbus::slave.cbVector[CB_WRITE_COILS] = fds::modbus::write_coils;
  fds::modbus::slave.cbVector[CB_WRITE_HOLDING_REGISTERS] = fds::modbus::write_holding_registers;

#if defined(_ARDUINO_SERIAL_DEBUG_) || defined(_ARDUINO_SERIAL_DEBUG_BAUD_)
  /* No Modbus */
#else 
  Serial.begin(RS485_BAUD);
  fds::modbus::slave.begin(RS485_BAUD);
#endif
}

char localbuffer[BUFFER_SIZE];

void loop() {
  tick = millis();
  
#if defined(_ARDUINO_SERIAL_DEBUG_) || defined(_ARDUINO_SERIAL_DEBUG_BAUD_)
  /* No Modbus */
#else 
  fds::modbus::slave.poll();
#endif

#ifdef PIN_COIL_LED_1
  if(fds::modbus::coil1) {
    ledcoil1.cycle();
  }
#endif
#ifdef PIN_COIL_LED_2
  if(fds::modbus::coil2) {
    ledcoil2.cycle();
  }
#endif

  if(tickdisplay.is(tick)) {
    if(*fds::modbus::writemsg1 == '\0' && *fds::modbus::writemsg2 == '\0') {
      if(tick >= write_msg_lifetime) {
        memset(localbuffer, ' ', 10);
        sprintf(localbuffer, "1 %d", fds::modbus::holdingregistry1);
        fds::format::assign(0, localbuffer, BUFFER_SIZE);
        memset(localbuffer, ' ', 10);
        sprintf(localbuffer, "2 %d", fds::modbus::holdingregistry2);
        fds::format::assign(1, localbuffer, BUFFER_SIZE);
        pointer = fds::format::get(0);
        ASDLN(pointer);
        pointer = fds::format::get(1);
        ASDLN(pointer);
      }
    } else {
      write_msg_lifetime = tick + DURATION_WRITE_MSG;
      if(*fds::modbus::writemsg1 != '\0') {
        fds::format::assign(0, fds::modbus::writemsg1, BUFFER_SIZE);
        *fds::modbus::writemsg1 = '\0';
      }
      if(*fds::modbus::writemsg2 != '\0') {
        fds::format::assign(1, fds::modbus::writemsg2, BUFFER_SIZE);
        *fds::modbus::writemsg2 = '\0';
      }
    }
    fds::display::lines(2);
  }

  fds::display::loop();
}
