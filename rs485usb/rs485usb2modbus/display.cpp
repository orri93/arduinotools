#include <gatlformat.h>

#include "display.h"
#include "format.h"
#include "fds-logo.h"

namespace gatl = ::gos::atl;
namespace grmf = ::gos::rs485::modbus::format;

namespace gos {
namespace rs485 {
namespace modbus {
namespace display {

namespace last {
int request, response;
}

Oled oled;
Two two(oled);
void start() {
  oled.U8g2->begin();
  gatl::display::synchronous::logo(
    oled,
    fds_logo_width,
    fds_logo_height,
    fds_logo_bits);
  last::request = INT_MAX;
  last::response = INT_MAX;
  gatl::format::integer<int>(grmf::first, 0);
  gatl::format::integer<int>(grmf::second, 0);
  two.display(grmf::first, grmf::second);
}
void show(const int& request, const int& response) {
  if ((request != 0 && request != last::request) || response != last::response){
    if (request != last::request) {
      gatl::format::integer<int>(grmf::first, request);
      last::request = request;
    }
    if (response != last::response) {
      gatl::format::integer<int>(grmf::second, response);
      last::response = response;
    }
    two.display(grmf::first, grmf::second);
  }
  two.loop();
}
} // namespace display
} // namespace modbus
} // namespace rs485
} // namespace gos
