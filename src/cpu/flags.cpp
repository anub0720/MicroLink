#include "microlink/cpu/flags.hpp"

namespace microlink::cpu {

void Flags::reset() noexcept {
    zero_ = false;
    sign_ = false;
    carry_ = false;
    parity_ = false;
}

bool Flags::zero() const noexcept {
    return zero_;
}

bool Flags::sign() const noexcept {
    return sign_;
}

bool Flags::carry() const noexcept {
    return carry_;
}

bool Flags::parity() const noexcept {
    return parity_;
}

void Flags::set_zero(bool value) noexcept {
    zero_ = value;
}

void Flags::set_sign(bool value) noexcept {
    sign_ = value;
}

void Flags::set_carry(bool value) noexcept {
    carry_ = value;
}

void Flags::set_parity(bool value) noexcept {
    parity_ = value;
}

Byte Flags::pack() const noexcept {
    Byte value = 0;

    if (sign_) {
        value |= 0x80;
    }

    if (zero_) {
        value |= 0x40;
    }

    if (parity_) {
        value |= 0x04;
    }

    if (carry_) {
        value |= 0x01;
    }

    return value;
}

void Flags::unpack(Byte value) noexcept {
    sign_ = (value & 0x80U) != 0;
    zero_ = (value & 0x40U) != 0;
    parity_ = (value & 0x04U) != 0;
    carry_ = (value & 0x01U) != 0;
}

} // namespace microlink::cpu