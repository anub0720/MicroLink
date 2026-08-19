#pragma once

#include "microlink/cpu/types.hpp"

namespace microlink::device {

class Device {
public:
    virtual ~Device() = default;

    virtual bool handles(cpu::Word address) const noexcept = 0;

    virtual cpu::Byte read(cpu::Word address) noexcept = 0;

    virtual void write(
        cpu::Word address,
        cpu::Byte value
    ) noexcept = 0;

    virtual void tick() noexcept = 0;
};

} // namespace microlink::device