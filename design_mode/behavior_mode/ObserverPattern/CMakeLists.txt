cmake_minimum_required(VERSION 3.16)
project(ObserverPattern)

set(CMAKE_CXX_STANDARD 14)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/dist)

add_subdirectory(Sync)
add_subdirectory(Async)

add_executable(ObserverPattern main.cpp)
target_link_libraries(ObserverPattern sync async1)
target_link_libraries(ObserverPattern fmt pthread)
