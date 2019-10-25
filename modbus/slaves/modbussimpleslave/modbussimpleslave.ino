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
 * ********************************************************/

#include <ModbusSlave.h>
#include <FixedPoints.h>
#include <FixedPointsCommon.h>
#include <FixedPoints/SFixed.h>

#include <gatlmodbus.h>

#include "pinuno.h"

#define MODBUS_SLAVE_ID         1
#define RS485_BAUD           9600

namespace gatl = ::gos::atl;

namespace gos {
namespace modbus {

typedef SQ15x16 Fixed;

namespace objects {
bool coil_01 = false, coil_02 = false;
bool discrete_01 = false, discrete_02 = false;
namespace input {
uint16_t ui16_01 = 0, ui16_02 = 0;
int32_t i32_01 = 0, i32_02 = 0;
float float_01 = 0.0, float_02 = 0.0;
Fixed fixed_01 = 0.0, fixed_02 = 0.0;
}
namespace holding {
uint16_t ui16_01 = 0, ui16_02 = 0;
int32_t i32_01 = 0, i32_02 = 0;
float float_01 = 0.0, float_02 = 0.0;
Fixed fixed_01 = 0.0, fixed_02 = 0.0;
}
}

namespace binding {
gatl::modbus::Bindings<bool> coil;
gatl::modbus::Bindings<bool> discrete;
namespace input {
gatl::modbus::Bindings<uint16_t> ui16;
gatl::modbus::Bindings<int32_t> i32;
gatl::modbus::Bindings<float> f;
gatl::modbus::Bindings<Fixed> fixed;
}
namespace holding {
gatl::modbus::Bindings<uint16_t> ui16;
gatl::modbus::Bindings<int32_t> i32;
gatl::modbus::Bindings<float> f;
gatl::modbus::Bindings<Fixed> fixed;
}

}

namespace led {
uint8_t next, last = LOW;
}
uint8_t index;
uint16_t address;

static uint8_t result;

Modbus slave(MODBUS_SLAVE_ID, PIN_RS485_MODBUS_TE);

/* 0x01 Read Coils */
uint8_t read_coils(
  uint8_t fc,
  uint16_t startaddress,
  uint16_t length) {
  result = STATUS_OK;
  gatl::modbus::coil::access(
    binding::coil,
    slave,
    startaddress,
    length);
  return result;
}

/* 0x02 Read Discrete Inputs */
uint8_t read_discrete_inputs(
  uint8_t fc,
  uint16_t startaddress,
  uint16_t length) {
  result = STATUS_OK;
  gatl::modbus::discrete::access(
    binding::discrete,
    slave,
    startaddress,
    length);
  return result;
}

/* 0x03 Read Multiple Holding Registers */
uint8_t read_holding_registers(
  uint8_t fc,
  uint16_t startaddress,
  uint16_t length) {
  result = STATUS_OK;
  gatl::modbus::registers::access(
    binding::holding::ui16,
    slave,
    startaddress,
    length);
  gatl::modbus::two::access(
    binding::holding::i32,
    slave,
    startaddress,
    length);
  gatl::modbus::two::access(
    binding::holding::f,
    slave,
    startaddress,
    length);
  gatl::modbus::two::access(
    binding::holding::fixed,
    slave,
    startaddress,
    length);
  return result;
}

/* 0x04 Read Input Registers */
uint8_t read_input_registers(
  uint8_t fc,
  uint16_t startaddress,
  uint16_t length) {
  result = STATUS_OK;
  gatl::modbus::registers::access(
    binding::input::ui16,
    slave,
    startaddress,
    length);
  gatl::modbus::two::access(
    binding::input::i32,
    slave,
    startaddress,
    length);
  gatl::modbus::two::access(
    binding::input::f,
    slave,
    startaddress,
    length);
  gatl::modbus::two::access(
    binding::input::fixed,
    slave,
    startaddress,
    length);
  return result;
}

/* 0x05 Write Single Coil and 0x0f Write Multiple Coils */
uint8_t write_coils(
  uint8_t fc,
  uint16_t startaddress,
  uint16_t length) {
  result = STATUS_OK;
  gatl::modbus::coil::assign(
    binding::coil,
    slave,
    startaddress,
    length);
  return result;
}

/* 0x06 Write Single Holding Register and 0x10 Write Multiple Holding Registers */
uint8_t write_holding_registers(
  uint8_t fc,
  uint16_t startaddress,
  uint16_t length) {
  result = STATUS_OK;
  gatl::modbus::registers::assign(
    binding::holding::ui16,
    slave,
    startaddress,
    length);
  gatl::modbus::two::assign(
    binding::holding::i32,
    slave,
    startaddress,
    length);
  gatl::modbus::two::assign(
    binding::holding::f,
    slave,
    startaddress,
    length);
  gatl::modbus::two::assign(
    binding::holding::fixed,
    slave,
    startaddress,
    length);
  return result;
}

}
}

namespace gm = ::gos::modbus;
namespace gmo = ::gos::modbus::objects;
namespace gmb = ::gos::modbus::binding;
namespace gmbh = ::gos::modbus::binding::holding;
namespace gmbi = ::gos::modbus::binding::input;

void begin() {
  index = 1; address = 0;
  gatl::modbus::create(gmb::coil, 2, address, &gmo::coil_01);
  gatl::modbus::add(gmb::coil, index, address, &gmo::coil_02);

  index = 1; address = 0;
  gatl::modbus::create(gmb::discrete, 2, address, &gmo::discrete_01);
  gatl::modbus::add(gmb::discrete, index, address, &gmo::discrete_02);

  index = 1; address = 0;
  gatl::modbus::create(gmbh::ui16, 2, address, &gmo::holding::ui16_01);
  gatl::modbus::add(gmbh::ui16, index, address, &gmo::holding::ui16_02);
  index = 1;
  gatl::modbus::create(gmbh::i32, 2, address, &gmo::holding::i32_01);
  gatl::modbus::add(gmbh::i32, index, address, &gmo::holding::i32_02);
  index = 1;
  gatl::modbus::create(gmbh::f, 2, address, &gmo::holding::float_01);
  gatl::modbus::add(gmbh::f, index, address, &gmo::holding::float_02);
  index = 1;
  gatl::modbus::create(gmbh::fixed, 2, address, &gmo::holding::fixed_01);
  gatl::modbus::add(gmbh::fixed, index, address, &gmo::holding::fixed_02);

  index = 1; address = 0;
  gatl::modbus::create(gmbi::ui16, 2, address, &gmo::input::ui16_01);
  gatl::modbus::add(gmbi::ui16, index, address, &gmo::input::ui16_02);
  index = 1;
  gatl::modbus::create(gmbi::i32, 2, address, &gmo::input::i32_01);
  gatl::modbus::add(gmbi::i32, index, address, &gmo::input::i32_02);
  index = 1;
  gatl::modbus::create(gmbi::f, 2, address, &gmo::input::float_01);
  gatl::modbus::add(gmbi::f, index, address, &gmo::input::float_02);
  index = 1;
  gatl::modbus::create(gmbi::fixed, 2, address, &gmo::input::fixed_01);
  gatl::modbus::add(gmbi::fixed, index, address, &gmo::input::fixed_02);

  // RS485 control pin must be output
  pinMode(PIN_RS485_MODBUS_TE, OUTPUT);

#ifdef PIN_COIL_LED_1
  pinMode(PIN_COIL_LED_1, OUTPUT);
  digitalWrite(PIN_COIL_LED_1, LOW);
#endif

  gm::slave.cbVector[CB_READ_COILS] = gm::read_coils;
  gm::slave.cbVector[CB_READ_DISCRETE_INPUTS] = gm::read_discrete_inputs;
  gm::slave.cbVector[CB_READ_INPUT_REGISTERS] = gm::read_input_registers;
  gm::slave.cbVector[CB_READ_HOLDING_REGISTERS] = gm::read_holding_registers;
  gm::slave.cbVector[CB_WRITE_COILS] = gm::write_coils;
  gm::slave.cbVector[CB_WRITE_HOLDING_REGISTERS] = gm::write_holding_registers;

  Serial.begin(RS485_BAUD);
  g::modbus::slave.begin(RS485_BAUD);
}

void loop() {
#ifdef PIN_COIL_LED_1
  gm::led::next = gmo::coil_01 ? HIGH : LOW;
  if (gm::led::next != gm::led::last) {
    digitalWrite(PIN_COIL_LED_1, gm::led::next);
    gm::led::last = gm::led::next;
  }
#endif
#ifdef PIN_INPUT_REGISTRY_1
  gmo::input::ui16_01 = analogRead(PIN_INPUT_REGISTRY_1);
#endif
#ifdef PIN_INPUT_REGISTRY_2
  gmo::input::ui16_02 = analogRead(PIN_INPUT_REGISTRY_2);
#endif
  gmo::discrete_01 = gmo::coil_02;
  gmo::discrete_02 = gmo::coil_01;
  gmo::input::ui16_01 = gmo::holding::ui16_02;
  gmo::input::ui16_02 = gmo::holding::ui16_01;
  gmo::input::i32_01 = gmo::holding::i32_02;
  gmo::input::i32_02 = gmo::holding::i32_01;
  gmo::input::float_01 = gmo::holding::float_02;
  gmo::input::float_02 = gmo::holding::float_01;
  gmo::input::fixed_01 = gmo::holding::fixed_02;
  gmo::input::fixed_02 = gmo::holding::fixed_01;

  gm::slave.poll();
}

