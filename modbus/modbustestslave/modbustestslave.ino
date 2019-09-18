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

typedef SQ15x16 fpsq;

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
  HoldingRegistry2 = 0x0001,
  HoldingRegistry1FpLb = 0x0002,
  HoldingRegistry1FpHb = 0x0003,
  HoldingRegistry2FpLb = 0x0004,
  HoldingRegistry2FpHb = 0x0005,
};

enum InputRegistry {
  InputRegistry1 = 0x0000,
  InputRegistry2 = 0x0001,
  InputRegistry1FpLb = 0x0002,
  InputRegistry1FpHb = 0x0003,
  InputRegistry2FpLb = 0x0004,
  InputRegistry2FpHb = 0x0005,
};

static uint16_t address, i;

bool coil1 = false, coil2 = true;
uint16_t holdingregistry1 = 0x00, holdingregistry2 = 0xff;

static uint8_t result;

Modbus slave(MODBUS_SLAVE_ID, PIN_RS485_MODBUS_TE);

fpsq tofp(const uint16_t& value, const uint16_t& maximum) {
  return static_cast<fpsq>(value) / static_cast<fpsq>(maximum);
}

uint16_t fplb(const uint16_t& value, const uint16_t& maximum) {
  fpsq fixed = tofp(value, maximum);
  uint16_t result;
  memcpy(static_cast<void*>(&result), static_cast<const void*>(&fixed), 2);
  return result;
}

uint16_t fphb(const uint16_t& value, const uint16_t& maximum) {
  fpsq fixed = tofp(value, maximum);
  uint16_t result;
  memcpy(
    static_cast<void*>(&result),
    static_cast<const void*>(reinterpret_cast<char*>(&fixed) + 2),
    2);
  return result;
}

/* 0x01 Read Coils */
uint8_t read_coils(uint8_t fc, uint16_t startaddress, uint16_t length) {
#ifdef PIN_LED_MODBUS_READ
  digitalWrite(PIN_LED_MODBUS_READ, HIGH);
#endif
  result = STATUS_OK;
  for (i = 0; i < length; i++) {
    address = startaddress + i;
    switch (address) {
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
  for (i = 0; i < length; i++) {
    address = startaddress + i;
    switch (address) {
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
  for (i = 0; i < length; i++) {
    address = startaddress + i;
    switch (address) {
    case HoldingRegistry1:
      slave.writeRegisterToBuffer(i, holdingregistry1);
      break;
    case HoldingRegistry2:
      slave.writeRegisterToBuffer(i, holdingregistry2);
      break;
    case HoldingRegistry1FpLb:
      //slave.writeRegisterToBuffer(i, );
      break;
    case HoldingRegistry1FpHb:
      //slave.writeRegisterToBuffer(i, );
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
  for (i = 0; i < length; i++) {
    address = startaddress + i;
    switch (address) {
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
  bool show = false;
  for (i = 0; i < length; i++) {
    address = startaddress + i;
    switch (address) {
    case Coil1:
      coil1 = slave.readCoilFromBuffer(i);
      display::show::coil(display::line::one, coil1);
      show = true;
#ifdef PIN_COIL_LED_1
      if (!coil1) {
        digitalWrite(PIN_COIL_LED_1, LOW);
      }
#endif
      break;
    case Coil2:
      coil2 = slave.readCoilFromBuffer(i);
      display::show::coil(display::line::two, coil2);
      show = true;
#ifdef PIN_COIL_LED_2
      if (!coil2) {
        digitalWrite(PIN_COIL_LED_2, LOW);
      }
#endif
      break;
    default:
      result = STATUS_ILLEGAL_DATA_ADDRESS;
      break;
    }
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
uint8_t write_holding_registers(uint8_t fc, uint16_t startaddress, uint16_t length) {
#ifdef PIN_LED_MODBUS_WRITE
  digitalWrite(PIN_LED_MODBUS_WRITE, HIGH);
#endif
  result = STATUS_OK;
  bool show = false;
  for (i = 0; i < length; i++) {
    address = startaddress + i;
    switch (address) {
    case HoldingRegistry1:
      holdingregistry1 = slave.readRegisterFromBuffer(i);
      show = true;
#ifdef PIN_HOLDING_REGISTRY_1
      if (holdingregistry1 >= 0 && holdingregistry1 <= 255) {
        analogWrite(PIN_HOLDING_REGISTRY_1, holdingregistry1);
      }
#endif
      break;
    case HoldingRegistry2:
      holdingregistry2 = slave.readRegisterFromBuffer(i);
      show = true;
#ifdef PIN_HOLDING_REGISTRY_2
      if (holdingregistry2 >= 0 && holdingregistry2 <= 255) {
        analogWrite(PIN_HOLDING_REGISTRY_2, holdingregistry2);
      }
#endif
      break;
    default:
      result = STATUS_ILLEGAL_DATA_ADDRESS;
      break;
    }
  }
  if (show) {
    display::show::registry();
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
  gm::display::begin();
  int sizeoffpsq = sizeof(fpsq);
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
  pinMode(PIN_DISCRETE_INPUTS_2, INPUT);

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
      sizeof(TEXT_COIL_HIGH));
  }
  else {
    ::gos::atl::format::message(
      *buffer,
      TEXT_COIL_LOW,
      sizeof(TEXT_COIL_LOW));
  }
}
}
}
}
}
