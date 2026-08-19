#pragma once

#include "microlink/cpu/types.hpp"

namespace microlink::cpu {

enum class RegisterId : Byte {
    A  = 0,
    R0 = 1,
    R1 = 2,
    R2 = 3,
    R3 = 4,
    R4 = 5,
    R5 = 6
};

constexpr Byte to_byte(RegisterId register_id) noexcept {
    return static_cast<Byte>(register_id);
}

constexpr bool is_valid_register(Byte value) noexcept {
    return value <= to_byte(RegisterId::R5);
}

constexpr RegisterId register_from_byte(Byte value) {
    return static_cast<RegisterId>(value);
}

} // namespace microlink::cpu