#include "microlink/cpu/alu.hpp"

namespace microlink::cpu {

namespace {

bool calculate_parity(Byte value) noexcept {
    bool parity = true;

    while (value != 0) {
        parity = !parity;
        value &= static_cast<Byte>(value - 1U);
    }

    return parity;
}

void update_result_flags(Flags& flags, Byte result) noexcept {
    flags.set_zero(result == 0);
    flags.set_sign((result & 0x80U) != 0);
    flags.set_parity(calculate_parity(result));
}

} // namespace

ALUResult ALU::execute(
    ALUOperation operation,
    Byte lhs,
    Byte rhs,
    const Flags& previous_flags
) {
    ALUResult result{0, previous_flags};

    switch (operation) {
    case ALUOperation::Add: {
        const auto sum =
            static_cast<std::uint16_t>(lhs) +
            static_cast<std::uint16_t>(rhs);

        result.value = static_cast<Byte>(sum & 0xFFU);
        result.flags.set_carry(sum > 0xFFU);
        break;
    }

    case ALUOperation::Subtract:
    case ALUOperation::Compare:
        result.value = static_cast<Byte>(lhs - rhs);
        result.flags.set_carry(lhs < rhs);
        break;

    case ALUOperation::And:
        result.value = static_cast<Byte>(lhs & rhs);
        result.flags.set_carry(false);
        break;

    case ALUOperation::Or:
        result.value = static_cast<Byte>(lhs | rhs);
        result.flags.set_carry(false);
        break;

    case ALUOperation::Xor:
        result.value = static_cast<Byte>(lhs ^ rhs);
        result.flags.set_carry(false);
        break;
    }

    update_result_flags(result.flags, result.value);

    return result;
}

} // namespace microlink::cpu