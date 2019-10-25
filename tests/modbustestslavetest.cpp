#include <mutex>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <Arduino.h>

#include <gos/utils/expect.h>

#include <modbus/slaves/modbustestslave/modbustestslave.ino>

namespace gmdd = ::gos::modbus::display::details;

class ModbusSlaveTestFixture : public ::testing::Test {
public:
  ModbusSlaveTestFixture() :
    arduinomock(nullptr),
    serialmock(nullptr) {
  }

  void SetUp() override {
    arduinomock = arduinoMockInstance();
    serialmock = serialMockInstance();
    if (gmdd::oled.U8g2 == nullptr) {
      gmdd::oled.U8g2 = new U8g2;
    }
  }

  void TearDown() override {
    releaseArduinoMock();
    releaseSerialMock();
    if (gmdd::oled.U8g2) {
      delete gmdd::oled.U8g2;
      gmdd::oled.U8g2 = nullptr;
    }
  }

  ArduinoMock* arduinomock;
  HardwareSerialMock* serialmock;

  std::mutex mutex;
};

TEST_F(ModbusSlaveTestFixture, Fp) {
  const uint16_t cv = 93;
  const uint16_t max = 255;
  const ::gos::modbus::fpsq cfp = 
    static_cast<double>(cv) / static_cast<double>(max);
  
  ::gos::modbus::fpsq fpv;
  uint16_t v16b;

  fpv = ::gos::modbus::tofp(cv, max);
  GOS_ARDUINO_TESTING_EQ_FP(cfp, fpv);

  v16b = ::gos::modbus::fromfp(fpv, max);
  EXPECT_TRUE(abs(static_cast<int32_t>(cv) - static_cast<int32_t>(v16b)) <= 1);
}

TEST_F(ModbusSlaveTestFixture, Setup) {
  EXPECT_CALL(*(gmdd::oled.U8g2), begin()).
    Times(testing::Exactly(1));
  /* Logo */
  EXPECT_CALL(*(gmdd::oled.U8g2), firstPage()).Times(testing::Exactly(1));
  EXPECT_CALL(*(gmdd::oled.U8g2), drawXBMP).Times(testing::Exactly(1));
  EXPECT_CALL(*(gmdd::oled.U8g2), nextPage()).Times(testing::Exactly(1))
    .WillOnce(testing::Return(0));

  /* 6 Sin Led * 3 * (pinMode, analogWrite, delay, digitalWrite, delay) */
  //EXPECT_CALL(*arduinomock, pinMode).Times(testing::AtLeast(6));
  //EXPECT_CALL(*arduinomock, analogWrite).Times(testing::AtLeast(3 * 6));
  //EXPECT_CALL(*arduinomock, delay).Times(testing::AtLeast(3 * 6));
  //EXPECT_CALL(*arduinomock, digitalWrite).Times(testing::AtLeast(3 * 6));
  EXPECT_CALL(*arduinomock, delay).Times(testing::AtLeast(3 * 6));

  /* Holding registry 2 * (analogWrite) */
  EXPECT_CALL(*arduinomock, analogWrite).Times(testing::AtLeast(2));

  /* RS485 control and Discrete Inputs pins */
  //EXPECT_CALL(*arduinomock, pinMode).Times(testing::AtLeast(1));
  //EXPECT_CALL(*arduinomock, pinMode).Times(testing::AtLeast(2));

  /* Modbus begin */
  EXPECT_CALL(*arduinomock, pinMode).Times(testing::AtLeast(1));
  EXPECT_CALL(*arduinomock, digitalWrite).Times(testing::AtLeast(1));

  EXPECT_CALL(*serialmock, begin(RS485_BAUD)).Times(testing::Exactly(1));
  EXPECT_CALL(*serialmock, setTimeout(0)).Times(testing::Exactly(1));
  EXPECT_CALL(*serialmock, flush()).Times(testing::Exactly(1));
  EXPECT_CALL(*serialmock, availableForWrite()).Times(testing::Exactly(1));
  EXPECT_CALL(*arduinomock, micros).Times(testing::Exactly(1));

  setup();
}
