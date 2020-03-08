#include <gatlmodbus.h>

#include "rs485io.h"

#define MODBUS_REQUEST_START_COUNT     6
#define MODBUS_REQUEST_LENGTH_INDEX    6
#define MODBUS_REQUEST_PAYLOAD_INDEX   7
#define MODBUS_REQUEST_MAXIMUM_REDUCED \
  (MODBUS_REQUEST_PAYLOAD_INDEX + MODBUS_CRC_LENGTH + 1)

#define MODBUS_RESPONSE_START_COUNT     2
#define MODBUS_RESPONSE_SINGLE_COUNT    4
#define MODBUS_RESPONSE_SINGLE_INDEX    2
#define MODBUS_RESPONSE_LENGTH_INDEX    2
#define MODBUS_RESPONSE_PAYLOAD_INDEX   3
#define MODBUS_RESPONSE_EXCEPTION_INDEX 2

#define MODBUS_RESPONSE_MAXIMUM_REDUCED \
  (MODBUS_RESPONSE_LENGTH_INDEX + MODBUS_CRC_LENGTH + 1)


namespace gos {
namespace rs485 {
namespace modbus {
namespace read {
int request(Stream& stream, uint8_t* buffer, const size_t& maximum) {
  int length = 0;
  size_t count = 0;
  if (stream.available()) {
    count = stream.readBytes(buffer, MODBUS_REQUEST_START_COUNT);
    if (count == MODBUS_REQUEST_START_COUNT) {
      switch (buffer[MODBUS_FUNCTION_CODE_INDEX]) {
      case MODBUS_FC_READ_COILS:
      case MODBUS_FC_READ_DISCRETE_INPUT:
      case MODBUS_FC_READ_INPUT_REGISTERS:
      case MODBUS_FC_READ_HOLDING_REGISTERS:
      case MODBUS_FC_WRITE_COIL:
      case MODBUS_FC_WRITE_REGISTER:
        length = MODBUS_REQUEST_START_COUNT;
        break;
      case MODBUS_FC_WRITE_MULTIPLE_COILS:
      case MODBUS_FC_WRITE_MULTIPLE_REGISTERS:
        length = stream.read();
        if (length > 0 && length < maximum - MODBUS_REQUEST_MAXIMUM_REDUCED) {
          buffer[MODBUS_REQUEST_LENGTH_INDEX] = (uint8_t)(length);
          count = stream.readBytes(
            buffer + MODBUS_REQUEST_PAYLOAD_INDEX,
            length);
          if (count == length) {
            length += MODBUS_REQUEST_PAYLOAD_INDEX;
          } else {
            return -500 - count;
          }
        } else {
          return -400 - length;
        }
        break;
      default:
        return -300;
      }
    } else {
      return -200 - count;
    }
  }
  if (length == 0) {
    return length;
  } else {
    count = stream.readBytes(buffer + length, MODBUS_CRC_LENGTH);
    if (count == MODBUS_CRC_LENGTH) {
      return length + MODBUS_CRC_LENGTH;
    } else {
      return -600 - count;
    }
  }
  return -100;
}
int response(Stream& stream, uint8_t* buffer, const size_t& maximum) {
  int available, length = 0;
  size_t count = 0;
  if ((available = stream.available()) > 0) {
    count = stream.readBytes(buffer, MODBUS_RESPONSE_START_COUNT);
    if (count == MODBUS_RESPONSE_START_COUNT) {
      if (buffer[MODBUS_FUNCTION_CODE_INDEX] & 0x80 == 0) {
        switch (buffer[MODBUS_FUNCTION_CODE_INDEX]) {
        case MODBUS_FC_READ_COILS:
        case MODBUS_FC_READ_DISCRETE_INPUT:
        case MODBUS_FC_READ_INPUT_REGISTERS:
        case MODBUS_FC_READ_HOLDING_REGISTERS:
          length = stream.read();
          if (length > 0 && length < maximum - MODBUS_RESPONSE_MAXIMUM_REDUCED) {
            buffer[MODBUS_RESPONSE_LENGTH_INDEX] = (uint8_t)(length);
            count = stream.readBytes(
              buffer + MODBUS_RESPONSE_PAYLOAD_INDEX,
              length);
            if (count == length) {
              length += MODBUS_RESPONSE_PAYLOAD_INDEX;
            } else {
              return -600 - count;
            }
          } else {
            return -500 - length;
          }
          break;
        case MODBUS_FC_WRITE_COIL:
        case MODBUS_FC_WRITE_REGISTER:
        case MODBUS_FC_WRITE_MULTIPLE_COILS:
        case MODBUS_FC_WRITE_MULTIPLE_REGISTERS:
          count = stream.readBytes(
            buffer + MODBUS_RESPONSE_SINGLE_INDEX,
            MODBUS_RESPONSE_SINGLE_COUNT);
          if (count == MODBUS_RESPONSE_SINGLE_COUNT) {
            length = MODBUS_RESPONSE_START_COUNT + MODBUS_RESPONSE_SINGLE_COUNT;
          } else {
            return -400 - count;
          }
          break;
        default:
          return -300;
        }
      } else {
        length = stream.read();
        if (length > 0) {
          buffer[MODBUS_RESPONSE_EXCEPTION_INDEX] = (uint8_t)(length);
          length = MODBUS_RESPONSE_START_COUNT + 1;
        } else {
          return -700 - length;
        }
      }
    } else {
      return -200  - count;
    }
  }
  if (length == 0) {
    return length;
  } else {
    count = stream.readBytes(buffer + length, MODBUS_CRC_LENGTH);
    if (count == MODBUS_CRC_LENGTH) {
      return length + MODBUS_CRC_LENGTH;
    } else {
      return -800 - count;
    }
  }
  return -100;
}
} // namespace read
} // namespace modbus
} // namespace rs485
} // namespace gos
