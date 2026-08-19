#include <array>
#include <gtest/gtest.h>

#include "microlink/cpu/decoder.hpp"

using namespace microlink::cpu;

TEST(Decoder, RejectsEmptyInput) {
    const std::array<Byte, 0> bytes{};

    EXPECT_THROW(
        Decoder::decode(bytes),
        std::runtime_error
    );
}

TEST(Decoder, DecodesNop) {
    const std::array<Byte, 1> bytes{
        to_byte(Opcode::NOP)
    };

    const auto result = Decoder::decode(bytes);

    EXPECT_EQ(result.instruction.opcode(), Opcode::NOP);
    EXPECT_EQ(result.bytes_consumed, 1);
}

TEST(Decoder, DecodesMovRegisterToRegister) {
    const std::array<Byte, 5> bytes{
        to_byte(Opcode::MOV),

        0x00,
        to_byte(RegisterId::A),

        0x00,
        to_byte(RegisterId::R1)
    };

    const auto result = Decoder::decode(bytes);

    EXPECT_EQ(result.instruction.opcode(), Opcode::MOV);

    EXPECT_EQ(
        result.instruction.operand(0).type,
        OperandType::Register
    );

    EXPECT_EQ(
        result.instruction.operand(0).register_id(),
        RegisterId::A
    );

    EXPECT_EQ(
        result.instruction.operand(1).type,
        OperandType::Register
    );

    EXPECT_EQ(
        result.instruction.operand(1).register_id(),
        RegisterId::R1
    );

    EXPECT_EQ(result.bytes_consumed, 5);
}

TEST(Decoder, DecodesMovImmediateToRegister) {
    const std::array<Byte, 5> bytes{
        to_byte(Opcode::MOV),

        0x00,
        to_byte(RegisterId::A),

        0x01,
        42
    };

    const auto result = Decoder::decode(bytes);

    EXPECT_EQ(result.instruction.opcode(), Opcode::MOV);

    EXPECT_EQ(
        result.instruction.operand(0).register_id(),
        RegisterId::A
    );

    EXPECT_EQ(
        result.instruction.operand(1).type,
        OperandType::Immediate
    );

    EXPECT_EQ(
        result.instruction.operand(1).value,
        42
    );

    EXPECT_EQ(result.bytes_consumed, 5);
}

TEST(Decoder, DecodesLoad) {
    const std::array<Byte, 6> bytes{
        to_byte(Opcode::LOAD),

        0x00,
        to_byte(RegisterId::R2),

        0x02,
        0x20,
        0x40
    };

    const auto result = Decoder::decode(bytes);

    EXPECT_EQ(result.instruction.opcode(), Opcode::LOAD);

    EXPECT_EQ(
        result.instruction.operand(0).register_id(),
        RegisterId::R2
    );

    EXPECT_EQ(
        result.instruction.operand(1).type,
        OperandType::Address
    );

    EXPECT_EQ(
        result.instruction.operand(1).value,
        0x2040
    );

    EXPECT_EQ(result.bytes_consumed, 6);
}

TEST(Decoder, DecodesStore) {
    const std::array<Byte, 6> bytes{
        to_byte(Opcode::STORE),

        0x00,
        to_byte(RegisterId::R3),

        0x02,
        0x30,
        0x40
    };

    const auto result = Decoder::decode(bytes);

    EXPECT_EQ(result.instruction.opcode(), Opcode::STORE);

    EXPECT_EQ(
        result.instruction.operand(0).register_id(),
        RegisterId::R3
    );

    EXPECT_EQ(
        result.instruction.operand(1).type,
        OperandType::Address
    );

    EXPECT_EQ(
        result.instruction.operand(1).value,
        0x3040
    );

    EXPECT_EQ(result.bytes_consumed, 6);
}

TEST(Decoder, DecodesArithmeticInstruction) {
    const std::array<Byte, 3> bytes{
        to_byte(Opcode::ADD),

        0x00,
        to_byte(RegisterId::R4)
    };

    const auto result = Decoder::decode(bytes);

    EXPECT_EQ(result.instruction.opcode(), Opcode::ADD);

    EXPECT_EQ(
        result.instruction.operand(0).register_id(),
        RegisterId::R4
    );

    EXPECT_EQ(result.bytes_consumed, 3);
}

TEST(Decoder, DecodesJump) {
    const std::array<Byte, 4> bytes{
        to_byte(Opcode::JMP),

        0x02,
        0x40,
        0x20
    };

    const auto result = Decoder::decode(bytes);

    EXPECT_EQ(result.instruction.opcode(), Opcode::JMP);

    EXPECT_EQ(
        result.instruction.operand(0).type,
        OperandType::Address
    );

    EXPECT_EQ(
        result.instruction.operand(0).value,
        0x4020
    );

    EXPECT_EQ(result.bytes_consumed, 4);
}

TEST(Decoder, DecodesPushAndPop) {
    const std::array<Byte, 3> push_bytes{
        to_byte(Opcode::PUSH),

        0x00,
        to_byte(RegisterId::R5)
    };

    const auto push_result =
        Decoder::decode(push_bytes);

    EXPECT_EQ(
        push_result.instruction.opcode(),
        Opcode::PUSH
    );

    EXPECT_EQ(
        push_result.instruction.operand(0).register_id(),
        RegisterId::R5
    );

    const std::array<Byte, 3> pop_bytes{
        to_byte(Opcode::POP),

        0x00,
        to_byte(RegisterId::R5)
    };

    const auto pop_result =
        Decoder::decode(pop_bytes);

    EXPECT_EQ(
        pop_result.instruction.opcode(),
        Opcode::POP
    );

    EXPECT_EQ(
        pop_result.instruction.operand(0).register_id(),
        RegisterId::R5
    );
}

TEST(Decoder, DecodesIoInstruction) {
    const std::array<Byte, 4> bytes{
        to_byte(Opcode::IN),

        0x02,
        0x80,
        0x10
    };

    const auto result = Decoder::decode(bytes);

    EXPECT_EQ(result.instruction.opcode(), Opcode::IN);

    EXPECT_EQ(
        result.instruction.operand(0).type,
        OperandType::Address
    );

    EXPECT_EQ(
        result.instruction.operand(0).value,
        0x8010
    );

    EXPECT_EQ(result.bytes_consumed, 4);
}

TEST(Decoder, OperandBytesCanHaveOpcodeValues) {
    const std::array<Byte, 5> bytes{
        to_byte(Opcode::MOV),

        0x00,
        to_byte(RegisterId::A),

        0x01,
        to_byte(Opcode::HALT)
    };

    const auto result = Decoder::decode(bytes);

    EXPECT_EQ(
        result.instruction.opcode(),
        Opcode::MOV
    );

    EXPECT_EQ(
        result.instruction.operand(1).type,
        OperandType::Immediate
    );

    EXPECT_EQ(
        result.instruction.operand(1).value,
        to_byte(Opcode::HALT)
    );

    EXPECT_EQ(result.bytes_consumed, 5);
}

TEST(Decoder, RejectsUnknownOpcode) {
    const std::array<Byte, 1> bytes{
        0xFF
    };

    EXPECT_THROW(
        Decoder::decode(bytes),
        std::runtime_error
    );
}

TEST(Decoder, RejectsTruncatedMov) {
    const std::array<Byte, 3> bytes{
        to_byte(Opcode::MOV),
        0x00,
        to_byte(RegisterId::A)
    };

    EXPECT_THROW(
        Decoder::decode(bytes),
        std::runtime_error
    );
}

TEST(Decoder, RejectsInvalidRegister) {
    const std::array<Byte, 3> bytes{
        to_byte(Opcode::ADD),
        0x00,
        0xFF
    };

    EXPECT_THROW(
        Decoder::decode(bytes),
        std::runtime_error
    );
}

TEST(Decoder, RejectsInvalidOperandType) {
    const std::array<Byte, 3> bytes{
        to_byte(Opcode::ADD),
        0xFF,
        0x00
    };

    EXPECT_THROW(
        Decoder::decode(bytes),
        std::runtime_error
    );
}

TEST(Decoder, RejectsInvalidMovDestination) {
    const std::array<Byte, 5> bytes{
        to_byte(Opcode::MOV),

        0x01,
        42,

        0x00,
        to_byte(RegisterId::R1)
    };

    EXPECT_THROW(
        Decoder::decode(bytes),
        std::runtime_error
    );
}

TEST(Decoder, RejectsInvalidLoadAddressType) {
    const std::array<Byte, 6> bytes{
        to_byte(Opcode::LOAD),

        0x00,
        to_byte(RegisterId::A),

        0x01,
        0x20,
        0x40
    };

    EXPECT_THROW(
        Decoder::decode(bytes),
        std::runtime_error
    );
}