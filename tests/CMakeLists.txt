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
  ${gos_arduino_sketches_unit_testing_include}
  "${gos_arduino_libraries_dir}/GosTemplates/src"
  "${gos_arduino_unit_testing_sublibraries_dir}/arduinotick/src")

target_link_libraries(ExecutArduinoToolsTests
  ${gos_arduino_sketches_unit_testing_target_link_libraries}
  libarduinotick)

add_test(NAME SketchesTestBoardTests COMMAND ExecuteArduinoSketchesTestBoardTests)
