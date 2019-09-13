#include <mutex>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <Arduino.h>

#include <modbus/modbustestslave/modbustestslave.ino>

class Rs485TestFixture : public ::testing::Test {
public:
  void SetUp() override {
    arduinomock = arduinoMockInstance();
  }

  void TearDown() override {
    releaseArduinoMock();
  }

  ArduinoMock* arduinomock;

  std::mutex mutex;
};

TEST_F(Rs485TestFixture, Setup) {
  EXPECT_CALL(*(oled.U8g2), begin()).
    Times(testing::AtLeast(1));

  setup();
}
