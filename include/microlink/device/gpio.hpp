#pragma once

#include "microlink/device/device.hpp"

namespace microlink::device {

class GPIO final : public Device {
public:
    static constexpr cpu::Word DataAddress = 0xF000;
    static constexpr cpu::Word DirectionAddress = 0xF001;

    GPIO();

    void reset() noexcept;

    bool handles(
        cpu::Word address
    ) const noexcept override;

    cpu::Byte read(
        cpu::Word address
    ) noexcept override;

    void write(
        cpu::Word address,
        cpu::Byte value
    ) noexcept override;

    void tick() noexcept override;

    cpu::Byte data() const noexcept;
    cpu::Byte direction() const noexcept;

private:
    cpu::Byte data_{0};
    cpu::Byte direction_{0};
};

} // namespace microlink::device