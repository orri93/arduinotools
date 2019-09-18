cmake_minimum_required(VERSION 3.1...3.15)

project(ArduinoTools VERSION 1.0 DESCRIPTION "GOS Arduino Tools" LANGUAGES CXX)

set(gos_arduino_tools_unit_testing_include
  ${gos_arduino_sketches_unit_testing_include}
  "${CMAKE_CURRENT_SOURCE_DIR}/modbus/modbustestslave"
  "${CMAKE_CURRENT_SOURCE_DIR}")

add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
add_compile_definitions(FIXED_POINTS_NO_RANDOM)
add_compile_definitions(FIXED_POINTS_NO_CONSTANTS)

add_subdirectory(tests)
