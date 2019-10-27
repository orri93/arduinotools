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
#include <FixedPoints.h>
#include <FixedPointsCommon.h>
#include <FixedPoints/SFixed.h>

#include <arduinosinled.h>
#include <gatldisplay.h>
#include <gatlmodbus.h>
#include <gatlformat.h>

#include "pinuno.h"
#include "fds-modbus-test-slave-logo.h"

#define MODBUS_SLAVE_ID         1
#define RS485_BAUD           9600

#define DURATION_MSG         3000

#define TEXT_ID_A "1: "
#define TEXT_ID_B "2: "

#define TEXT_COIL_HIGH "High"
#define TEXT_COIL_LOW "Low"

namespace gatl = ::gos::atl;
namespace gatlb = ::gos::atl::binding;

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

namespace gos {
namespace modbus {

typedef SQ15x16 Fixed;

namespace display {
enum class line { one, two };
void begin();
void loop();
namespace show {
void logo();
void error(const char* message);
void display(bool message = false);
void registry();
void coil(const line& line, const bool& status);
}
}

namespace lastaddress {
uint16_t coil = 0x0000, discrete = 0x0000, input = 0x0000, holding = 0x0000;
}

uint8_t from, to;
bool coil1 = false, coil2 = true;
bool discrete1 = false, discrete2 = true;
uint16_t analog1 = 0x00, analog2 = 0x00;
uint16_t holdingregistry1 = 0x00, holdingregistry2 = 0xff;

uint16_t address;
uint8_t index;

namespace objects {
namespace input {
int32_t i32_01 = 0, i32_02 = 0;
float float_01 = 0.0, float_02 = 0.0;
Fixed fixed_01 = 0.0, fixed_02 = 0.0;
}
namespace holding {
int32_t i32_01 = 0, i32_02 = 0;
float float_01 = 0.0, float_02 = 0.0;
Fixed fixed_01 = 0.0, fixed_02 = 0.0;
}
}

namespace binding {
gatl::binding::reference<bool> coil;
gatl::binding::reference<bool> discrete;
namespace input {
gatl::binding::reference<uint16_t> ui16;
gatl::binding::reference<int32_t> i32;
gatl::binding::reference<float> f;
gatl::binding::reference<Fixed> fixed;
}
namespace holding {
gatl::binding::reference<uint16_t> ui16;
gatl::binding::reference<int32_t> i32;
gatl::binding::reference<float> f;
gatl::binding::reference<Fixed> fixed;
}
void create() {
  lastaddress::coil = gatlb::create<bool, uint16_t, uint8_t>(coil,
    0,    // Address
    2,    // Count
    1);   // Size
  gatl::binding::set<bool, uint16_t, uint8_t>(coil, 0, &coil1);
  gatl::binding::set<bool, uint16_t, uint8_t>(coil, 1, &coil2);

  lastaddress::discrete = gatlb::create<bool, uint16_t, uint8_t>(discrete,
    0,    // Address
    2,    // Count
    1);   // Size
  gatl::binding::set<bool, uint16_t, uint8_t>(discrete, 0, &discrete1);
  gatl::binding::set<bool, uint16_t, uint8_t>(discrete, 1, &discrete2);

  address = gatlb::create<uint16_t, uint16_t, uint8_t>(input::ui16,
    0,    // Address
    2,    // Count
    1);   // Size
  gatl::binding::set<uint16_t, uint16_t, uint8_t>(input::ui16, 0, &analog1);
  gatl::binding::set<uint16_t, uint16_t, uint8_t>(input::ui16, 1, &analog2);
  address = gatlb::create<int32_t, uint16_t, uint8_t>(input::i32,
    address,  // Address
    2,        // Count
    2);       // Size
  gatlb::set<int32_t, uint16_t, uint8_t>(input::i32, 0, &objects::input::i32_01);
  gatlb::set<int32_t, uint16_t, uint8_t>(input::i32, 1, &objects::input::i32_02);
  address = gatlb::create<float, uint16_t, uint8_t>(input::f,
    address,  // Address
    2,        // Count
    2);       // Size
  gatlb::set<float, uint16_t, uint8_t>(input::f, 0, &objects::input::float_01);
  gatlb::set<float, uint16_t, uint8_t>(input::f, 1, &objects::input::float_02);
  lastaddress::input = gatlb::create<Fixed, uint16_t, uint8_t>(input::fixed,
    address,  // Address
    2,        // Count
    2);       // Size
  gatlb::set<Fixed, uint16_t, uint8_t>(input::fixed, 0, &objects::input::fixed_01);
  gatlb::set<Fixed, uint16_t, uint8_t>(input::fixed, 1, &objects::input::fixed_02);

  address = gatlb::create<uint16_t, uint16_t, uint8_t>(holding::ui16,
    0,    // Address
    2,    // Count
    1);   // Size
  gatl::binding::set<uint16_t, uint16_t, uint8_t>(holding::ui16, 0, &holdingregistry1);
  gatl::binding::set<uint16_t, uint16_t, uint8_t>(holding::ui16, 1, &holdingregistry2);
  address = gatlb::create<int32_t, uint16_t, uint8_t>(holding::i32,
    address,  // Address
    2,        // Count
    2);       // Size
  gatlb::set<int32_t, uint16_t, uint8_t>(holding::i32, 0, &objects::holding::i32_01);
  gatlb::set<int32_t, uint16_t, uint8_t>(holding::i32, 1, &objects::holding::i32_02);
  address = gatlb::create<float, uint16_t, uint8_t>(holding::f,
    address,  // Address
    2,        // Count
    2);       // Size
  gatlb::set<float, uint16_t, uint8_t>(holding::f, 0, &objects::holding::float_01);
  gatlb::set<float, uint16_t, uint8_t>(holding::f, 1, &objects::holding::float_02);
  lastaddress::holding = gatlb::create<Fixed, uint16_t, uint8_t>(holding::fixed,
    address,  // Address
    2,        // Count
    2);       // Size
  gatlb::set<Fixed, uint16_t, uint8_t>(holding::fixed, 0, &objects::holding::fixed_01);
  gatlb::set<Fixed, uint16_t, uint8_t>(holding::fixed, 1, &objects::holding::fixed_02);
}
}

static uint8_t result;

Modbus slave(MODBUS_SLAVE_ID, PIN_RS485_MODBUS_TE);

/* 0x01 Read Coils */
uint8_t read_coils(
  uint8_t fc,
  uint16_t startaddress,
  uint16_t length) {
#ifdef PIN_LED_MODBUS_READ
  digitalWrite(PIN_LED_MODBUS_READ, HIGH);
#endif
  result = STATUS_OK;
  if (startaddress + length < lastaddress::coil) {
    gatl::modbus::coil::access(binding::coil, slave, startaddress, length);
  } else {
    result = STATUS_ILLEGAL_DATA_ADDRESS;
  }
#ifdef PIN_LED_MODBUS_READ
  digitalWrite(PIN_LED_MODBUS_READ, LOW);
#endif
  return result;
}

/* 0x02 Read Discrete Inputs */
uint8_t read_discrete_inputs(
  uint8_t fc,
  uint16_t startaddress,
  uint16_t length) {
#ifdef PIN_LED_MODBUS_READ
  digitalWrite(PIN_LED_MODBUS_READ, HIGH);
#endif
  result = STATUS_OK;
  if (startaddress + length < lastaddress::discrete) {
    gatl::modbus::discrete::access(binding::discrete, slave, startaddress, length);
  }
  else {
    result = STATUS_ILLEGAL_DATA_ADDRESS;
  }
#ifdef PIN_LED_MODBUS_READ
  digitalWrite(PIN_LED_MODBUS_READ, LOW);
#endif
  return result;
}

/* 0x03 Read Multiple Holding Registers */
uint8_t read_holding_registers(
  uint8_t fc,
  uint16_t startaddress,
  uint16_t length) {
#ifdef PIN_LED_MODBUS_READ
  digitalWrite(PIN_LED_MODBUS_READ, HIGH);
#endif
  result = STATUS_OK;
  if (startaddress + length < lastaddress::holding) {
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
  } else {
    result = STATUS_ILLEGAL_DATA_ADDRESS;
  }
#ifdef PIN_LED_MODBUS_READ
  digitalWrite(PIN_LED_MODBUS_READ, LOW);
#endif
  return result;
}

/* 0x04 Read Input Registers */
uint8_t read_input_registers(
  uint8_t fc,
  uint16_t startaddress,
  uint16_t length) {
#ifdef PIN_LED_MODBUS_READ
  digitalWrite(PIN_LED_MODBUS_READ, HIGH);
#endif
  result = STATUS_OK;
  if (startaddress + length < lastaddress::input) {
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
  } else {
    result = STATUS_ILLEGAL_DATA_ADDRESS;
  }
#ifdef PIN_LED_MODBUS_READ
  digitalWrite(PIN_LED_MODBUS_READ, LOW);
#endif
  return result;
}

/* 0x05 Write Single Coil and 0x0f Write Multiple Coils */
uint8_t write_coils(
  uint8_t fc,
  uint16_t startaddress,
  uint16_t length) {
#ifdef PIN_LED_MODBUS_WRITE
  digitalWrite(PIN_LED_MODBUS_WRITE, HIGH);
#endif
  result = STATUS_OK;
  bool show = false;
  if (startaddress + length < lastaddress::coil) {
    gatl::modbus::coil::assign(binding::coil, slave, startaddress, length, from, to);
    if (from == 0) {
      show = true;
#ifdef PIN_COIL_LED_1
      if (!coil1) {
        digitalWrite(PIN_COIL_LED_1, LOW);
      }
#endif
    }
    if (to == 2) {
      show = true;
#ifdef PIN_COIL_LED_2
      if (!coil2) {
        digitalWrite(PIN_COIL_LED_2, LOW);
      }
#endif
    }
  } else {
    result = STATUS_ILLEGAL_DATA_ADDRESS;
  }
  if (show) {
    display::show::display(true);
  }
#ifdef PIN_LED_MODBUS_WRITE
  digitalWrite(PIN_LED_MODBUS_WRITE, LOW);
#endif
  return result;
}

/* 0x06 Write Single Holding Register and 0x10 Write Multiple Holding Registers */
uint8_t write_holding_registers(
  uint8_t fc,
  uint16_t startaddress,
  uint16_t length) {
#ifdef PIN_LED_MODBUS_WRITE
  digitalWrite(PIN_LED_MODBUS_WRITE, HIGH);
#endif
  result = STATUS_OK;
  uint16_t previous1 = holdingregistry1, previous2 = holdingregistry2;
  if (startaddress + length < lastaddress::holding) {
    gatl::modbus::registers::assign(
      binding::holding::ui16,
      slave,
      startaddress,
      length,
      from,
      to);
    gatl::modbus::two::assign(
      binding::holding::i32,
      slave,
      startaddress,
      length,
      from,
      to);
    gatl::modbus::two::assign(
      binding::holding::f,
      slave,
      startaddress,
      length,
      from,
      to);
    gatl::modbus::two::assign(
      binding::holding::fixed,
      slave,
      startaddress,
      length,
      from,
      to);
  } else {
    result = STATUS_ILLEGAL_DATA_ADDRESS;
  }
  if(previous1 != holdingregistry1 || previous2 != holdingregistry2) {
    display::show::registry();
#ifdef PIN_HOLDING_REGISTRY_1
    if (holdingregistry1 >= 0 && holdingregistry1 <= 255) {
      analogWrite(PIN_HOLDING_REGISTRY_1, holdingregistry1);
    }
#endif
#ifdef PIN_HOLDING_REGISTRY_2
    if (holdingregistry2 >= 0 && holdingregistry2 <= 255) {
      analogWrite(PIN_HOLDING_REGISTRY_2, holdingregistry2);
    }
#endif
  }
#ifdef PIN_LED_MODBUS_WRITE
  digitalWrite(PIN_LED_MODBUS_WRITE, LOW);
#endif
  return result;
}

}
}

namespace g = ::gos;
namespace gm = ::gos::modbus;

void setup() {
  g::modbus::binding::create();

  gm::display::begin();
  int sizeoffpsq = sizeof(::gos::modbus::Fixed);
  if (sizeoffpsq == 4) {
    gm::display::show::logo();
  }
  else {
    char message[12];
    sprintf(message, "FPE %d", sizeoffpsq);
    gm::display::show::error(message);
    while (true) {
      delay(10);
    }
  }

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
  if (g::modbus::holdingregistry1 >= 0 && g::modbus::holdingregistry1 <= 255) {
    analogWrite(PIN_HOLDING_REGISTRY_1, g::modbus::holdingregistry1);
  }
#endif
#ifdef PIN_HOLDING_REGISTRY_2
  if (g::modbus::holdingregistry2 >= 0 && g::modbus::holdingregistry2 <= 255) {
    analogWrite(PIN_HOLDING_REGISTRY_2, g::modbus::holdingregistry2);
  }
#endif

  // RS485 control pin must be output
  pinMode(PIN_RS485_MODBUS_TE, OUTPUT);

#ifdef PIN_DISCRETE_INPUTS_1
  pinMode(PIN_DISCRETE_INPUTS_1, INPUT);
#endif
#ifdef PIN_DISCRETE_INPUTS_2
  pinMode(PIN_DISCRETE_INPUTS_2, INPUT);
#endif

  gm::slave.cbVector[CB_READ_COILS] = gm::read_coils;
  gm::slave.cbVector[CB_READ_DISCRETE_INPUTS] = gm::read_discrete_inputs;
  gm::slave.cbVector[CB_READ_INPUT_REGISTERS] = gm::read_input_registers;
  gm::slave.cbVector[CB_READ_HOLDING_REGISTERS] = gm::read_holding_registers;
  gm::slave.cbVector[CB_WRITE_COILS] = gm::write_coils;
  gm::slave.cbVector[CB_WRITE_HOLDING_REGISTERS] = gm::write_holding_registers;

#if defined(_ARDUINO_SERIAL_DEBUG_) || defined(_ARDUINO_SERIAL_DEBUG_BAUD_)
  /* No Modbus */
#else 
  Serial.begin(RS485_BAUD);
  g::modbus::slave.begin(RS485_BAUD);
#endif

  delay(3000);

  gm::display::show::registry();
}

void loop() {
  g::modbus::slave.poll();

#ifdef PIN_COIL_LED_1
  if (g::modbus::coil1) {
    ledcoil1.cycle();
  }
#endif
#ifdef PIN_COIL_LED_2
  if (g::modbus::coil2) {
    ledcoil2.cycle();
  }
#endif

#ifdef PIN_DISCRETE_INPUTS_1
  g::modbus::discrete1 = digitalRead(PIN_DISCRETE_INPUTS_1);
#endif
#ifdef PIN_DISCRETE_INPUTS_2
  g::modbus::discrete2 = digitalRead(PIN_DISCRETE_INPUTS_2);
#endif

  g::modbus::objects::input::i32_01 = g::modbus::objects::holding::i32_02;
  g::modbus::objects::input::i32_02 = g::modbus::objects::holding::i32_01;
  g::modbus::objects::input::float_01 = g::modbus::objects::holding::float_02;
  g::modbus::objects::input::float_02 = g::modbus::objects::holding::float_01;
  g::modbus::objects::input::fixed_01 = g::modbus::objects::holding::fixed_02;
  g::modbus::objects::input::fixed_02 = g::modbus::objects::holding::fixed_01;

  gm::display::loop();
}

namespace gos {
namespace modbus {

namespace display {
namespace details {
::gos::atl::format::option::Number option;
::gos::atl::buffer::Holder<> buffer1(11);
::gos::atl::buffer::Holder<> buffer2(11);
::gos::atl::buffer::Holder<> ida(TEXT_ID_A, sizeof(TEXT_ID_A));
::gos::atl::buffer::Holder<> idb(TEXT_ID_B, sizeof(TEXT_ID_B));
::gos::atl::display::Oled<> oled;
::gos::atl::display::asynchronous::line::Two<> twoline(oled);
::gos::atl::display::asynchronous::line::One<> oneline(oled);
unsigned long messagetick = 0;
}
void begin() {
  details::oled.U8g2->begin();
}
void loop() {
#ifdef PIN_INPUT_REGISTRY_1
  g::modbus::analog1 = analogRead(PIN_INPUT_REGISTRY_1);
#endif
#ifdef PIN_INPUT_REGISTRY_2
  g::modbus::analog2 = analogRead(PIN_INPUT_REGISTRY_2);
#endif

  if (
    details::messagetick > 0 &&
    millis() > (details::messagetick + DURATION_MSG)) {
    details::messagetick = 0;
    show::registry();
  }
  details::twoline.loop();
}
namespace show {
void logo() {
  ::gos::atl::display::synchronous::logo(
    details::oled,
    fds_modbus_test_slave_logo_width,
    fds_modbus_test_slave_logo_height,
    fds_modbus_test_slave_logo_bits);
}
void display(bool message) {
  details::twoline.display(details::buffer1, details::buffer2);
  if (message) {
    details::messagetick = millis();
  }
}
void error(const char* message) {
  ::gos::atl::display::synchronous::line::one(details::oled, message);
}
void registry() {
  ::gos::atl::format::integer(
    details::buffer1,
    holdingregistry1,
    &details::ida);
  ::gos::atl::format::integer(
    details::buffer2,
    holdingregistry2,
    &details::idb);
  display();
}
void coil(const line& line, const bool& status) {
  ::gos::atl::buffer::Holder<>* buffer = nullptr;
  switch (line) {
  case display::line::one:
    buffer = &details::buffer1;
    break;
  case display::line::two:
    buffer = &details::buffer2;
    break;
  }
  if (status) {
    ::gos::atl::format::message(
      *buffer,
      TEXT_COIL_HIGH,
      sizeof(TEXT_COIL_HIGH),
      &details::ida);
  }
  else {
    ::gos::atl::format::message(
      *buffer,
      TEXT_COIL_LOW,
      sizeof(TEXT_COIL_LOW),
      &details::idb);
  }
}
}
}
}
}
