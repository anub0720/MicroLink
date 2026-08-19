cmake_minimum_required(VERSION 3.20)

project(MicroLink
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

FetchContent_Declare(
    Crow
    GIT_REPOSITORY https://github.com/CrowCpp/Crow.git
    GIT_TAG v1.3.3
)

FetchContent_MakeAvailable(Crow)

add_library(microlink_cpu

    src/cpu/flags.cpp
    src/cpu/cpu_state.cpp
    src/cpu/alu.cpp
    src/cpu/decoder.cpp
    src/cpu/cpu.cpp
    src/cpu/executor.cpp
    src/cpu/memory.cpp

    src/runtime/runtime.cpp
    src/runtime/trace.cpp
    src/runtime/controller.cpp

    src/application/application.cpp
        src/device/device_bus.cpp
    src/device/gpio.cpp
    src/device/timer.cpp
    src/device/uart.cpp
)

target_include_directories(microlink_cpu
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

target_compile_options(microlink_cpu
    PRIVATE
        -Wall
        -Wextra
        -Wpedantic
        -Wconversion
        -Wshadow
)

enable_testing()

find_package(GTest REQUIRED)

add_executable(microlink_cpu_tests

    tests/cpu/cpu_tests.cpp
    tests/cpu/instruction_tests.cpp
    tests/cpu/decoder_tests.cpp
    tests/cpu/executor_tests.cpp
    tests/cpu/memory_tests.cpp

    tests/runtime/runtime_tests.cpp
    tests/runtime/trace_tests.cpp
    tests/runtime/controller_tests.cpp

    tests/application/application_tests.cpp
    tests/api/server_tests.cpp
)

target_link_libraries(microlink_cpu_tests
    PRIVATE
        microlink_cpu
        GTest::gtest_main
)

include(GoogleTest)

gtest_discover_tests(microlink_cpu_tests)

add_executable(microlink_server
    src/api/main.cpp
    src/api/server.cpp
)

target_link_libraries(microlink_server
    PRIVATE
        microlink_cpu
        Crow::Crow
        ws2_32
        mswsock
)