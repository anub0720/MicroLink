#pragma once

#include "microlink/cpu/types.hpp"

namespace microlink::cpu {

class Flags {
public:
    void reset() noexcept;

    bool zero() const noexcept;
    bool sign() const noexcept;
    bool carry() const noexcept;
    bool parity() const noexcept;

    void set_zero(bool value) noexcept;
    void set_sign(bool value) noexcept;
    void set_carry(bool value) noexcept;
    void set_parity(bool value) noexcept;

    Byte pack() const noexcept;
    void unpack(Byte value) noexcept;

private:
    bool zero_{false};
    bool sign_{false};
    bool carry_{false};
    bool parity_{false};
};

} // namespace microlink::cpu