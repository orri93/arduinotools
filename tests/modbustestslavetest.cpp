#include <mutex>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <Arduino.h>

#include <modbus/modbustestslave/modbustestslave.ino>

class ModbusSlaveTestFixture : public ::testing::Test {
public:
  void SetUp() override {
    arduinomock = arduinoMockInstance();
    if (oled.U8g2 == nullptr) {
      oled.U8g2 = new U8g2;
    }
  }

  void TearDown() override {
    releaseArduinoMock();
    if (oled.U8g2) {
      delete oled.U8g2;
      oled.U8g2 = nullptr;
    }
  }

  ArduinoMock* arduinomock;

  std::mutex mutex;
};

TEST_F(ModbusSlaveTestFixture, Setup) {
  EXPECT_CALL(*(oled.U8g2), begin()).
    Times(testing::AtLeast(1));

  setup();
}
