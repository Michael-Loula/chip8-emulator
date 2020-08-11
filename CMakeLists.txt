cmake_minimum_required(VERSION 3.10)

project(chip8-emulator)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${chip8-emulator_SOURCE_DIR}/cmake")
set(BIN_DIR ${chip8-emulator_SOURCE_DIR}/bin)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

add_executable(chip8-emulator src/main.cpp)
target_link_libraries(chip8-emulator ${SDL2_LIBRARIES})
install(TARGETS chip8-emulator RUNTIME DESTINATION ${BIN_DIR})
