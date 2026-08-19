#include <array>
#include <gtest/gtest.h>

#include "microlink/application/application.hpp"

using namespace microlink::cpu;
using namespace microlink::application;
using namespace microlink::runtime;

TEST(Application, StartsReady) {
    Application application;

    EXPECT_EQ(
        application.state(),
        RunState::Ready
    );
}

TEST(Application, LoadsAndExecutesProgram) {
    Application application;

    const std::array<Byte, 5> program{
        to_byte(Opcode::MOV),
        0x00,
        to_byte(RegisterId::A),
        0x01,
        42
    };

    application.load_program(program, 0x1000);
    application.step();

    EXPECT_EQ(
        application.cpu_state().accumulator(),
        42
    );

    EXPECT_EQ(
        application.trace().size(),
        1
    );
}

TEST(Application, RunsProgram) {
    Application application;

    const std::array<Byte, 10> program{
        to_byte(Opcode::MOV),
        0x00,
        to_byte(RegisterId::A),
        0x01,
        10,

        to_byte(Opcode::MOV),
        0x00,
        to_byte(RegisterId::R1),
        0x01,
        20
    };

    application.load_program(program, 0x1000);

    EXPECT_EQ(
        application.run(2),
        2
    );

    EXPECT_EQ(
        application.cpu_state().register_value(1),
        20
    );

    EXPECT_EQ(
        application.trace().size(),
        2
    );
}

TEST(Application, ResetClearsExecution) {
    Application application;

    const std::array<Byte, 1> program{
        to_byte(Opcode::NOP)
    };

    application.load_program(program, 0x1000);
    application.step();
    application.reset();

    EXPECT_EQ(
        application.state(),
        RunState::Ready
    );

    EXPECT_EQ(
        application.cpu_state().program_counter(),
        0
    );

    EXPECT_TRUE(
        application.trace().empty()
    );
}