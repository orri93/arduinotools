#ifndef RS485_USB_MODBUS_DISPLAY_H_
#define RS485_USB_MODBUS_DISPLAY_H_

#include <gatldisplay.h>

namespace gos {
namespace rs485 {
namespace modbus {
namespace display {
typedef ::gos::atl::display::Oled<> Oled;
typedef ::gos::atl::display::asynchronous::line::Two<> Two;
extern Oled oled;
extern Two two;
void start();
void show(const int& request, const int& response);
} // namespace display
} // namespace modbus
} // namespace rs485
} // namespace gos

#endif
