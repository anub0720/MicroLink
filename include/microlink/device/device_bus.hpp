#pragma once

#include "microlink/device/gpio.hpp"
#include "microlink/device/timer.hpp"
#include "microlink/device/uart.hpp"

namespace microlink::device {

class DeviceBus {
public:
    DeviceBus();

    void reset() noexcept;

    bool handles(
        cpu::Word address
    ) const noexcept;

    cpu::Byte read(
        cpu::Word address
    ) noexcept;

    void write(
        cpu::Word address,
        cpu::Byte value
    ) noexcept;

    void tick() noexcept;

    const GPIO& gpio() const noexcept;
    GPIO& gpio() noexcept;

    const UART& uart() const noexcept;
    UART& uart() noexcept;

    const Timer& timer() const noexcept;
    Timer& timer() noexcept;

private:
    GPIO gpio_{};
    UART uart_{};
    Timer timer_{};
};

} // namespace microlink::device