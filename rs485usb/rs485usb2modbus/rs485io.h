#ifndef RS485_USB_MODBUS_IO_H_
#define RS485_USB_MODBUS_IO_H_

#include <Arduino.h>
#include <Stream.h>

namespace gos {
namespace rs485 {
namespace modbus {
namespace read {
int request(Stream& stream, uint8_t* buffer, const size_t& maximum);
int response(Stream& stream, uint8_t* buffer, const size_t& maximum);
} // namespace read
} // namespace modbus
} // namespace rs485
} // namespace gos

#endif
