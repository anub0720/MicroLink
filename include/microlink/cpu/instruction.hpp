#pragma once

#include <array>
#include <cstddef>

#include "microlink/cpu/opcode.hpp"
#include "microlink/cpu/register_id.hpp"
#include "microlink/cpu/types.hpp"

namespace microlink::cpu {

enum class OperandType {
    None,
    Register,
    Immediate,
    Address
};

struct Operand {
    OperandType type{OperandType::None};
    Word value{0};

    static constexpr Operand none() noexcept {
        return {};
    }

    static constexpr Operand reg(RegisterId id) noexcept {
        return {
            OperandType::Register,
            to_byte(id)
        };
    }

    static constexpr Operand immediate(Byte value) noexcept {
        return {
            OperandType::Immediate,
            value
        };
    }

    static constexpr Operand address(Word value) noexcept {
        return {
            OperandType::Address,
            value
        };
    }

    constexpr RegisterId register_id() const noexcept {
        return register_from_byte(static_cast<Byte>(value));
    }
};

class Instruction {
public:
    static constexpr std::size_t MaxOperands = 2;

    Instruction() = default;

    explicit constexpr Instruction(Opcode opcode) noexcept
        : opcode_(opcode) {}

    constexpr Instruction(
        Opcode opcode,
        Operand operand1
    ) noexcept
        : opcode_(opcode),
          operands_{operand1} {}

    constexpr Instruction(
        Opcode opcode,
        Operand operand1,
        Operand operand2
    ) noexcept
        : opcode_(opcode),
          operands_{operand1, operand2} {}

    constexpr Opcode opcode() const noexcept {
        return opcode_;
    }

    constexpr const Operand& operand(std::size_t index) const noexcept {
        return operands_[index];
    }

private:
    Opcode opcode_{Opcode::NOP};
    std::array<Operand, MaxOperands> operands_{};
};

} // namespace microlink::cpu