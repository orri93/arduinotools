#ifndef RS485_USB_MODBUS_FORMAT_H_
#define RS485_USB_MODBUS_FORMAT_H_

#include <gatlbuffer.h>

namespace gos {
namespace rs485 {
namespace modbus {
namespace format {
typedef ::gos::atl::buffer::Holder<> Holder;
extern Holder first;
extern Holder second;
} // namespace format
} // namespace modbus
} // namespace rs485
} // namespace gos

#endif
