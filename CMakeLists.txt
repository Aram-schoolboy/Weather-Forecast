cmake_minimum_required(VERSION 3.20)
project(weather_forecast)

set(CMAKE_CXX_STANDARD 17)

if (WIN32)
    set(CMAKE_LIBRARY OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif()

add_subdirectory(src)
add_subdirectory(bin)


