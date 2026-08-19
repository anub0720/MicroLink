#include <gtest/gtest.h>

#include "microlink/cpu/instruction.hpp"

using namespace microlink::cpu;

TEST(Opcode, ConvertsToByte) {
    EXPECT_EQ(to_byte(Opcode::NOP), 0x80);
    EXPECT_EQ(to_byte(Opcode::LOAD), 0x82);
    EXPECT_EQ(to_byte(Opcode::RETI), 0x9B);
}

TEST(Instruction, CanRepresentNoOperandInstruction) {
    const Instruction instruction(Opcode::NOP);

    EXPECT_EQ(instruction.opcode(), Opcode::NOP);
    EXPECT_EQ(
        instruction.operand(0).type,
        OperandType::None
    );
}

TEST(Instruction, CanRepresentOneOperandInstruction) {
    const Operand operand{
        OperandType::Immediate,
        42
    };

    const Instruction instruction(
        Opcode::PUSH,
        operand
    );

    EXPECT_EQ(instruction.opcode(), Opcode::PUSH);
    EXPECT_EQ(
        instruction.operand(0).type,
        OperandType::Immediate
    );
    EXPECT_EQ(
        instruction.operand(0).value,
        42
    );
}

TEST(Instruction, CanRepresentTwoOperandInstruction) {
    const Operand destination{
        OperandType::Register,
        0
    };

    const Operand source{
        OperandType::Register,
        1
    };

    const Instruction instruction(
        Opcode::MOV,
        destination,
        source
    );

    EXPECT_EQ(instruction.opcode(), Opcode::MOV);

    EXPECT_EQ(
        instruction.operand(0).type,
        OperandType::Register
    );
    EXPECT_EQ(
        instruction.operand(0).value,
        0
    );

    EXPECT_EQ(
        instruction.operand(1).type,
        OperandType::Register
    );
    EXPECT_EQ(
        instruction.operand(1).value,
        1
    );
}

TEST(Instruction, CanRepresentSixteenBitAddress) {
    const Operand address{
        OperandType::Address,
        0x2040
    };

    const Instruction instruction(
        Opcode::LOAD,
        address
    );

    EXPECT_EQ(
        instruction.operand(0).type,
        OperandType::Address
    );

    EXPECT_EQ(
        instruction.operand(0).value,
        0x2040
    );
}