#pragma once

#include "microlink/device/device.hpp"

namespace microlink::device {

class Timer final : public Device {
public:
    static constexpr cpu::Word CounterAddress = 0xF020;
    static constexpr cpu::Word CompareAddress = 0xF021;
    static constexpr cpu::Word ControlAddress = 0xF022;

    Timer();

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

    cpu::Byte counter() const noexcept;
    cpu::Byte compare() const noexcept;
    bool enabled() const noexcept;
    bool interrupt_pending() const noexcept;

private:
    cpu::Byte counter_{0};
    cpu::Byte compare_{0};
    cpu::Byte control_{0};
    bool interrupt_pending_{false};
};

} // namespace microlink::device