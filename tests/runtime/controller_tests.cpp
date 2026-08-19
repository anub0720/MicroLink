#include <array>
#include <gtest/gtest.h>

#include "microlink/runtime/controller.hpp"

using namespace microlink::cpu;
using namespace microlink::runtime;

TEST(Controller, StartsReady) {
    Controller controller;

    EXPECT_EQ(
        controller.run_state(),
        RunState::Ready
    );
}

TEST(Controller, LoadingProgramMakesControllerReady) {
    Controller controller;

    const std::array<Byte, 1> program{
        to_byte(Opcode::NOP)
    };

    controller.load(program, 0x1000);

    EXPECT_EQ(
        controller.run_state(),
        RunState::Ready
    );
}

TEST(Controller, StepExecutesInstruction) {
    Controller controller;

    const std::array<Byte, 5> program{
        to_byte(Opcode::MOV),
        0x00,
        to_byte(RegisterId::A),
        0x01,
        42
    };

    controller.load(program, 0x1000);
    controller.step();

    EXPECT_EQ(
        controller.runtime().cpu().state().accumulator(),
        42
    );

    EXPECT_EQ(
        controller.run_state(),
        RunState::Paused
    );

    EXPECT_EQ(controller.trace().size(), 1);
}

TEST(Controller, RunExecutesProgram) {
    Controller controller;

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

    controller.load(program, 0x1000);

    const std::size_t executed =
        controller.run(2);

    EXPECT_EQ(executed, 2);
    EXPECT_EQ(controller.trace().size(), 2);

    EXPECT_EQ(
        controller.runtime().cpu().state().accumulator(),
        10
    );

    EXPECT_EQ(
        controller.runtime().cpu().state().register_value(1),
        20
    );

    EXPECT_EQ(
        controller.run_state(),
        RunState::Paused
    );
}

TEST(Controller, PauseChangesRunningState) {
    Controller controller;

    const std::array<Byte, 1> program{
        to_byte(Opcode::NOP)
    };

    controller.load(program, 0x1000);

    controller.run(1);
    controller.pause();

    EXPECT_EQ(
        controller.run_state(),
        RunState::Paused
    );
}

TEST(Controller, ResetReturnsToReady) {
    Controller controller;

    const std::array<Byte, 1> program{
        to_byte(Opcode::NOP)
    };

    controller.load(program, 0x1000);
    controller.step();
    controller.reset();

    EXPECT_EQ(
        controller.run_state(),
        RunState::Ready
    );

    EXPECT_TRUE(controller.trace().empty());
}