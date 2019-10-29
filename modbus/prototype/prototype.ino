/*
 * ********************************************************
 * Mudbus Slave Prototype V0.0.1
 * ********************************************************
 *
 * Slave id 1
 *
 * Holding Registry
 *   40001 0x0000 Holding Registry #1
 *   40002 0x0001 Holding Registry #2
 *
 * ********************************************************/

#include <EEPROM.h>

#include <ModbusSlave.h>

#include <gatleeprom.h>
#include <gatlmodbus.h>

#include "pinuno.h"

#define MODBUS_SLAVE_ID         1
#define RS485_BAUD           9600

namespace gatl = ::gos::atl;
namespace gatlb = ::gos::atl::binding;
namespace gatlm = ::gos::atl::modbus;
namespace gatle = ::gos::atl::eeprom;

namespace gos {
namespace modbus {
namespace prototype {

namespace lastaddress {
uint16_t holding = 0x0000;
}

uint8_t from, to;

uint16_t holdingregistry1 = 0x00, holdingregistry2 = 0xff;

namespace objects {
namespace holding {
int32_t i32_01 = 0, i32_02 = 0;
float float_01 = 0.0, float_02 = 0.0;
}
}

namespace binding {
namespace eeprom {
gatl::binding::reference<uint16_t, int> ui16;
gatl::binding::reference<int32_t, int> i32;
gatl::binding::reference<float, int> f;
void create() {
  int eepaddr = 0x0000;
  eepaddr = gatlb::create<uint16_t, int, uint8_t>(ui16,
    eepaddr,              // Address
    2,                    // Count
    sizeof(uint16_t);     // Size
  gatl::binding::set<uint16_t, int, uint8_t>(ui16, 0, &holdingregistry1);
  gatl::binding::set<uint16_t, int, uint8_t>(ui16, 1, &holdingregistry2);
  eepaddr = gatlb::create<int32_t, int, uint8_t>(i32,
    eepaddr,              // Address
    2,                    // Count
    sizeof(int32_t);      // Size
  gatlb::set<int32_t, int, uint8_t>(i32, 0, &objects::holding::i32_01);
  gatlb::set<int32_t, int, uint8_t>(i32, 1, &objects::holding::i32_02);
  eepaddr = gatlb::create<float, int, uint8_t>(f,
    eepaddr,              // Address
    2,                    // Count
    sizeof(float);        // Size
  gatlb::set<float, int, uint8_t>(f, 0, &objects::holding::float_01);
  gatlb::set<float, int, uint8_t>(f, 1, &objects::holding::float_02);
}
}

namespace modbus {
namespace holding {
gatl::binding::reference<uint16_t> ui16;
gatl::binding::reference<int32_t> i32;
gatl::binding::reference<float> f;
}
void create() {
  uint16_t modaddr = 0x0000;
  modaddr = gatlb::create<uint16_t, uint16_t, uint8_t>(holding::ui16,
    0,    // Address
    2,    // Count
    );   // Size
  gatl::binding::set<uint16_t, uint16_t, uint8_t>(holding::ui16, 0, &holdingregistry1);
  gatl::binding::set<uint16_t, uint16_t, uint8_t>(holding::ui16, 1, &holdingregistry2);
  modaddr = gatlb::create<int32_t, uint16_t, uint8_t>(holding::i32,
    modaddr,  // Address
    2,        // Count
    2);       // Size
  gatlb::set<int32_t, uint16_t, uint8_t>(holding::i32, 0, &objects::holding::i32_01);
  gatlb::set<int32_t, uint16_t, uint8_t>(holding::i32, 1, &objects::holding::i32_02);
  lastaddress::holding = gatlb::create<float, uint16_t, uint8_t>(holding::f,
    modaddr,  // Address
    2,        // Count
    2);       // Size
  gatlb::set<float, uint16_t, uint8_t>(holding::f, 0, &objects::holding::float_01);
  gatlb::set<float, uint16_t, uint8_t>(holding::f, 1, &objects::holding::float_02);
}
}
}

uint8_t result;

Modbus slave(MODBUS_SLAVE_ID, PIN_RS485_MODBUS_TE);

/* 0x01 Read Coils */
uint8_t read_coils(
  uint8_t fc,
  uint16_t startaddress,
  uint16_t length) {
  result = STATUS_OK;
#ifdef COILS
  if (startaddress + length <= lastaddress::coil) {
    gatl::modbus::coil::access(binding::coil, slave, startaddress, length);
  }
  else {
    result = STATUS_ILLEGAL_DATA_ADDRESS;
  }
#endif
  return result;
}

/* 0x02 Read Discrete Inputs */
uint8_t read_discrete_inputs(
  uint8_t fc,
  uint16_t startaddress,
  uint16_t length) {
  result = STATUS_OK;
  return result;
}

/* 0x03 Read Multiple Holding Registers */
uint8_t read_holding_registers(
  uint8_t fc,
  uint16_t startaddress,
  uint16_t length) {
  result = STATUS_OK;
  if (startaddress + length <= lastaddress::holding) {
    gatl::modbus::registers::access(
      binding::modbus::holding::ui16,
      slave,
      startaddress,
      length);
    gatl::modbus::two::access(
      binding::modbus::holding::i32,
      slave,
      startaddress,
      length);
    gatl::modbus::two::access(
      binding::modbus::holding::f,
      slave,
      startaddress,
      length);
  }
  else {
    result = STATUS_ILLEGAL_DATA_ADDRESS;
  }
  return result;
}

/* 0x04 Read Input Registers */
uint8_t read_input_registers(
  uint8_t fc,
  uint16_t startaddress,
  uint16_t length) {
  result = STATUS_OK;
  return result;
}

/* 0x05 Write Single Coil and 0x0f Write Multiple Coils */
uint8_t write_coils(
  uint8_t fc,
  uint16_t startaddress,
  uint16_t length) {
  result = STATUS_OK;
#ifdef COILS
  if (startaddress + length <= lastaddress::coil) {
    gatl::modbus::coil::assign(binding::coil, slave, startaddress, length, from, to);
  }
  else {
    result = STATUS_ILLEGAL_DATA_ADDRESS;
  }
  gatl::eeprom::write(eeprom::coil);
#endif
  return result;
}

/* 0x06 Write Single Holding Register and 0x10 Write Multiple Holding Registers */
uint8_t write_holding_registers(
  uint8_t fc,
  uint16_t startaddress,
  uint16_t length) {
  result = STATUS_OK;
  if (startaddress + length <= lastaddress::holding) {
    gatl::modbus::registers::assign(
      binding::modbus::holding::ui16,
      slave,
      startaddress,
      length,
      from,
      to);
    gatl::modbus::two::assign(
      binding::modbus::holding::i32,
      slave,
      startaddress,
      length,
      from,
      to);
    gatl::modbus::two::assign(
      binding::modbus::holding::f,
      slave,
      startaddress,
      length,
      from,
      to);
  }
  gatl::eeprom::write(binding::eeprom::ui16);
  gatl::eeprom::write(binding::eeprom::i32);
  gatl::eeprom::write(binding::eeprom::f);
  return result;
}

}
}
}

namespace gmp = ::gos::modbus::prototype;

void setup() {
  gmp::binding::eeprom::create();
  gatl::eeprom::read(gmp::binding::eeprom::ui16);
  gatl::eeprom::read(gmp::binding::eeprom::i32);
  gatl::eeprom::read(gmp::binding::eeprom::f);
  gmp::binding::modbus::create();

  // RS485 control pin must be output
  pinMode(PIN_RS485_MODBUS_TE, OUTPUT);

  gmp::slave.cbVector[CB_READ_COILS] = gmp::read_coils;
  gmp::slave.cbVector[CB_READ_DISCRETE_INPUTS] = gmp::read_discrete_inputs;
  gmp::slave.cbVector[CB_READ_INPUT_REGISTERS] = gmp::read_input_registers;
  gmp::slave.cbVector[CB_READ_HOLDING_REGISTERS] = gmp::read_holding_registers;
  gmp::slave.cbVector[CB_WRITE_COILS] = gmp::write_coils;
  gmp::slave.cbVector[CB_WRITE_HOLDING_REGISTERS] = gmp::write_holding_registers;

  Serial.begin(RS485_BAUD);
  gmp::slave.begin(RS485_BAUD);

  delay(3000);
}

void loop() {
  gmp::slave.poll();
}
