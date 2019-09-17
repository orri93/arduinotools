cmake_minimum_required(VERSION 3.8.2)

project(ArduinoTools VERSION 1.0
  DESCRIPTION "GOS Arduino tools Google unit tests"
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_PLATFORM_INDEPENDENT_CODE ON)

add_compile_definitions(GOS_ARDUINO_TEMPLATE_LIBRARY_CHECK_SIZE)
add_compile_definitions(GOS_ARDUINO_UNIT_TESTING)

add_executable(ExecutArduinoToolsTests
  modbustestslavetest.cpp)

include_directories(
  ${gos_arduino_tools_unit_testing_include})

target_link_libraries(ExecutArduinoToolsTests
  ${gos_arduino_sketches_unit_testing_target_link_libraries}
  libarduinotick)

add_test(NAME ModbusTestSlaveTests
  COMMAND ExecutArduinoToolsTests)
