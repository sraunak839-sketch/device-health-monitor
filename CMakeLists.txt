cmake_minimum_required(VERSION 3.10)
project(DeviceHealthMonitor CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(nlohmann_json 3.2.0 REQUIRED)

add_executable(health_monitor main.cpp)
target_link_libraries(health_monitor PRIVATE nlohmann_json::nlohmann_json)