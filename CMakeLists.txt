cmake_minimum_required(VERSION 3.27)
project(Ellis)

set(CMAKE_CXX_STANDARD 17)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

add_executable(ellisc main.cpp include/argparse.hpp)

include_directories(src)
add_subdirectory(src)

target_link_libraries(ellisc ellis)
add_subdirectory(Google_tests)

