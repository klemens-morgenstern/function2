cmake_minimum_required(VERSION 3.29)
project(function2)

find_package(Boost REQUIRED system)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)
add_executable(function2 main.cpp)
target_link_libraries(function2 PUBLIC Boost::system)