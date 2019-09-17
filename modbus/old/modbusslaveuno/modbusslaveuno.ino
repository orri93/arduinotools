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
 * MAX485 Wiring   Max485    Arduino UNO
 *                 1 RO      0 RX
 *                 2 RE      2 DI
 *                 3 DE      2 DI
 *                 4 DI      1 TX
 *                 5 GND       GND
 *                 6 A
 *                 7 B
 *                 8 VCC       5V
 * 
 * Display Wiring  Display   Arduino UNO
 *                 1 SDA     A4 (SDA)
 *                 2 SCL     A5 (SCL)
 *                 3 VCC     5V
 *                 4 GND     GND
 * 
 * ********************************************************/

#include <ModbusSlave.h>

#include <arduinotick.h>
#include <arduinoformat.h>
#include <arduinodisplay.h>

//#define _ARDUINO_SERIAL_DEBUG_BAUD_ 115200
#include <serialdebug.h>

#define PIN_RS485_MODBUS_TE     2

//#define PIN_LED_MODBUS_READ    5
//#define PIN_LED_MODBUS_WRITE   6

#define PIN_COIL_LED_1          7
#define PIN_COIL_LED_2          8

#define PIN_INDICATION_I2C_SDA A4
#define PIN_INDICATION_I2C_SCL A5

#define PIN_DISCRETE_INPUTS_1   3
#define PIN_DISCRETE_INPUTS_2   4

#define PIN_INPUT_REGISTRY_1   A0
#define PIN_INPUT_REGISTRY_2   A1

#define MODBUS_SLAVE_ID         1
#define RS485_BAUD           9600

#define INTERVAL_DISPLAY      100

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
#ifdef PIN_COIL_LED_1
      digitalWrite(PIN_COIL_LED_1, coil1 ? HIGH : LOW);
#endif
      break;
    case Coil2:
      coil2 = slave.readCoilFromBuffer(i);
#ifdef PIN_COIL_LED_2
      digitalWrite(PIN_COIL_LED_2, coil2 ? HIGH : LOW);
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
      break;
    case HoldingRegistry2:
      holdingregistry2 = slave.readRegisterFromBuffer(i);
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

unsigned long tick;

char* pointer;

void setup() {
  ARDUINO_SERIAL_DEBUG_SETUP();

  fds::format::setup();

  fds::display::u8g2.begin();

  // RS485 control pin must be output
  pinMode(PIN_RS485_MODBUS_TE, OUTPUT);

  pinMode(PIN_DISCRETE_INPUTS_1, INPUT);
  pinMode(PIN_DISCRETE_INPUTS_2, INPUT);

#ifdef PIN_COIL_LED_1
  pinMode(PIN_COIL_LED_1, OUTPUT);
  digitalWrite(PIN_COIL_LED_1, fds::modbus::coil1 ? HIGH : LOW);
#endif
#ifdef PIN_COIL_LED_2
  pinMode(PIN_COIL_LED_2, OUTPUT);
  digitalWrite(PIN_COIL_LED_2, fds::modbus::coil2 ? HIGH : LOW);
#endif

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

char localbuffer[16];

void loop() {
  tick = millis();
  
#if defined(_ARDUINO_SERIAL_DEBUG_) || defined(_ARDUINO_SERIAL_DEBUG_BAUD_)
  /* No Modbus */
#else 
  fds::modbus::slave.poll();
#endif

  if(tickdisplay.is(tick)) {
    memset(localbuffer, ' ', 10);
    sprintf(localbuffer, "1 %d", fds::modbus::holdingregistry1);
    fds::format::assign(0, localbuffer, 16);
    memset(localbuffer, ' ', 10);
    sprintf(localbuffer, "2 %d", fds::modbus::holdingregistry2);
    fds::format::assign(1, localbuffer, 16);
    pointer = fds::format::get(0);
    ASDLN(pointer);
    pointer = fds::format::get(1);
    ASDLN(pointer);
    fds::display::lines(2);
  }

  fds::display::loop();
}
