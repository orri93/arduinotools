#include <mutex>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <Arduino.h>

#include <modbus/modbustestslave/modbustestslave.ino>

namespace gmdd = ::gos::modbus::display::details;

class ModbusSlaveTestFixture : public ::testing::Test {
public:
  void SetUp() override {
    arduinomock = arduinoMockInstance();
    if (gmdd::oled.U8g2 == nullptr) {
      gmdd::oled.U8g2 = new U8g2;
    }
  }

  void TearDown() override {
    releaseArduinoMock();
    if (gmdd::oled.U8g2) {
      delete gmdd::oled.U8g2;
      gmdd::oled.U8g2 = nullptr;
    }
  }

  ArduinoMock* arduinomock;

  std::mutex mutex;
};

TEST_F(ModbusSlaveTestFixture, Setup) {
  EXPECT_CALL(*(gmdd::oled.U8g2), begin()).
    Times(testing::AtLeast(1));

  setup();
}
