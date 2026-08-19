#include "microlink/cpu/decoder.hpp"

#include <stdexcept>

#include "microlink/cpu/register_id.hpp"

namespace microlink::cpu {

namespace {

enum class EncodedOperandType : Byte {
    Register = 0x00,
    Immediate = 0x01,
    Address = 0x02
};

Byte read_byte(
    std::span<const Byte> bytes,
    std::size_t index
) {
    if (index >= bytes.size()) {
        throw std::runtime_error("Truncated instruction");
    }

    return bytes[index];
}

Word read_word(
    std::span<const Byte> bytes,
    std::size_t index
) {
    const Byte high = read_byte(bytes, index);
    const Byte low = read_byte(bytes, index + 1);

    return static_cast<Word>(
        (static_cast<Word>(high) << 8U) |
        static_cast<Word>(low)
    );
}

Operand decode_operand(
    std::span<const Byte> bytes,
    std::size_t& cursor
) {
    const Byte encoded_type = read_byte(bytes, cursor);
    ++cursor;

    switch (encoded_type) {
    case static_cast<Byte>(EncodedOperandType::Register): {
        const Byte register_value = read_byte(bytes, cursor);
        ++cursor;

        if (!is_valid_register(register_value)) {
            throw std::runtime_error("Invalid register");
        }

        return Operand::reg(
            register_from_byte(register_value)
        );
    }

    case static_cast<Byte>(EncodedOperandType::Immediate): {
        const Byte value = read_byte(bytes, cursor);
        ++cursor;

        return Operand::immediate(value);
    }

    case static_cast<Byte>(EncodedOperandType::Address): {
        const Word value = read_word(bytes, cursor);
        cursor += 2;

        return Operand::address(value);
    }

    default:
        throw std::runtime_error("Invalid operand type");
    }
}

} // namespace

DecodeResult Decoder::decode(std::span<const Byte> bytes) {
    if (bytes.empty()) {
        throw std::runtime_error("Cannot decode empty byte stream");
    }

    const Byte raw_opcode = bytes[0];

    switch (raw_opcode) {

    case to_byte(Opcode::NOP):
    case to_byte(Opcode::RET):
    case to_byte(Opcode::HALT):
    case to_byte(Opcode::EI):
    case to_byte(Opcode::DI):
    case to_byte(Opcode::RETI):
        return {
            Instruction(static_cast<Opcode>(raw_opcode)),
            1
        };

    case to_byte(Opcode::MOV): {
        std::size_t cursor = 1;

        const Operand destination =
            decode_operand(bytes, cursor);

        const Operand source =
            decode_operand(bytes, cursor);

        if (destination.type != OperandType::Register) {
            throw std::runtime_error(
                "MOV destination must be a register"
            );
        }

        if (source.type != OperandType::Register &&
            source.type != OperandType::Immediate) {
            throw std::runtime_error(
                "MOV source must be register or immediate"
            );
        }

        return {
            Instruction(
                Opcode::MOV,
                destination,
                source
            ),
            cursor
        };
    }

    case to_byte(Opcode::LOAD): {
        std::size_t cursor = 1;

        const Operand destination =
            decode_operand(bytes, cursor);

        const Operand address =
            decode_operand(bytes, cursor);

        if (destination.type != OperandType::Register) {
            throw std::runtime_error(
                "LOAD destination must be a register"
            );
        }

        if (address.type != OperandType::Address) {
            throw std::runtime_error(
                "LOAD address operand must be an address"
            );
        }

        return {
            Instruction(
                Opcode::LOAD,
                destination,
                address
            ),
            cursor
        };
    }

    case to_byte(Opcode::STORE): {
        std::size_t cursor = 1;

        const Operand source =
            decode_operand(bytes, cursor);

        const Operand address =
            decode_operand(bytes, cursor);

        if (source.type != OperandType::Register) {
            throw std::runtime_error(
                "STORE source must be a register"
            );
        }

        if (address.type != OperandType::Address) {
            throw std::runtime_error(
                "STORE address operand must be an address"
            );
        }

        return {
            Instruction(
                Opcode::STORE,
                source,
                address
            ),
            cursor
        };
    }

    case to_byte(Opcode::ADD):
    case to_byte(Opcode::SUB):
    case to_byte(Opcode::CMP):
    case to_byte(Opcode::AND):
    case to_byte(Opcode::OR):
    case to_byte(Opcode::XOR): {
        std::size_t cursor = 1;

        const Operand operand =
            decode_operand(bytes, cursor);

        if (operand.type != OperandType::Register) {
            throw std::runtime_error(
                "ALU operand must be a register"
            );
        }

        return {
            Instruction(
                static_cast<Opcode>(raw_opcode),
                operand
            ),
            cursor
        };
    }

    case to_byte(Opcode::INC):
    case to_byte(Opcode::DEC):
    case to_byte(Opcode::NOT):
    case to_byte(Opcode::PUSH):
    case to_byte(Opcode::POP): {
        std::size_t cursor = 1;

        const Operand operand =
            decode_operand(bytes, cursor);

        if (operand.type != OperandType::Register) {
            throw std::runtime_error(
                "Instruction operand must be a register"
            );
        }

        return {
            Instruction(
                static_cast<Opcode>(raw_opcode),
                operand
            ),
            cursor
        };
    }

    case to_byte(Opcode::JMP):
    case to_byte(Opcode::JZ):
    case to_byte(Opcode::JNZ):
    case to_byte(Opcode::JC):
    case to_byte(Opcode::JNC):
    case to_byte(Opcode::CALL): {
        std::size_t cursor = 1;

        const Operand address =
            decode_operand(bytes, cursor);

        if (address.type != OperandType::Address) {
            throw std::runtime_error(
                "Jump target must be an address"
            );
        }

        return {
            Instruction(
                static_cast<Opcode>(raw_opcode),
                address
            ),
            cursor
        };
    }

    case to_byte(Opcode::IN):
    case to_byte(Opcode::OUT): {
        std::size_t cursor = 1;

        const Operand address =
            decode_operand(bytes, cursor);

        if (address.type != OperandType::Address) {
            throw std::runtime_error(
                "I/O operand must be an address"
            );
        }

        return {
            Instruction(
                static_cast<Opcode>(raw_opcode),
                address
            ),
            cursor
        };
    }

    default:
        throw std::runtime_error("Unknown opcode");
    }
}

} // namespace microlink::cpu