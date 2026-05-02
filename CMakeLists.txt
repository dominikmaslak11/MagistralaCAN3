cmake_minimum_required(VERSION 3.16)
project(MagistralaCAN3 VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

include_directories(${CMAKE_SOURCE_DIR}/src)

find_package(Qt6 REQUIRED COMPONENTS Widgets Core Test)
find_package(Lua 5.3 REQUIRED)

include_directories(${LUA_INCLUDE_DIR})

set(SOURCES
    src/main.cpp
    src/can/can_worker.cpp
    src/can/socketcan_adapter.cpp
    src/can/socketcan_fd_adapter.cpp
    src/can/socketcan_j1939_adapter.cpp
    src/gui/can_frame_model.cpp
    src/gui/can_frame_detail_view.cpp
    src/gui/candump_exporter.cpp
    src/gui/candidate_list_view.cpp
    src/gui/byte_correlation_view.cpp
    src/gui/throttled_frame_model.cpp
    src/ml/assoc_learner.cpp
    src/ml/value_assoc_learner.cpp
    src/lua/lua_engine.cpp
    src/gateway/can_gateway.cpp
    src/gateway/rule_engine.cpp
    src/flow/flow_block.cpp
    src/flow/flow_blocks.cpp
    src/flow/flow_engine.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Widgets
    Qt6::Core
    ${LUA_LIBRARIES}
)
