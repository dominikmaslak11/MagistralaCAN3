cmake_minimum_required(VERSION 3.16)
project(MagistralaCAN3 VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

include_directories(${CMAKE_SOURCE_DIR}/src)

find_package(Qt6 REQUIRED COMPONENTS Widgets Core Test)

set(SOURCES
    src/main.cpp
    src/can/can_worker.cpp
    src/can/socketcan_adapter.cpp
    src/gui/can_frame_model.cpp
    src/gui/throttled_frame_model.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets Qt6::Core)
