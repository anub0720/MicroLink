#include "microlink/device/device_bus.hpp"

namespace microlink::device {

DeviceBus::DeviceBus() {
    reset();
}

void DeviceBus::reset() noexcept {
    gpio_.reset();
    uart_.reset();
    timer_.reset();
}

bool DeviceBus::handles(
    cpu::Word address
) const noexcept {
    return gpio_.handles(address) ||
           uart_.handles(address) ||
           timer_.handles(address);
}

cpu::Byte DeviceBus::read(
    cpu::Word address
) noexcept {
    if (gpio_.handles(address)) {
        return gpio_.read(address);
    }

    if (uart_.handles(address)) {
        return uart_.read(address);
    }

    if (timer_.handles(address)) {
        return timer_.read(address);
    }

    return 0;
}

void DeviceBus::write(
    cpu::Word address,
    cpu::Byte value
) noexcept {
    if (gpio_.handles(address)) {
        gpio_.write(address, value);
        return;
    }

    if (uart_.handles(address)) {
        uart_.write(address, value);
        return;
    }

    if (timer_.handles(address)) {
        timer_.write(address, value);
    }
}

void DeviceBus::tick() noexcept {
    gpio_.tick();
    uart_.tick();
    timer_.tick();
}

const GPIO& DeviceBus::gpio() const noexcept {
    return gpio_;
}

GPIO& DeviceBus::gpio() noexcept {
    return gpio_;
}

const UART& DeviceBus::uart() const noexcept {
    return uart_;
}

UART& DeviceBus::uart() noexcept {
    return uart_;
}

const Timer& DeviceBus::timer() const noexcept {
    return timer_;
}

Timer& DeviceBus::timer() noexcept {
    return timer_;
}

} // namespace microlink::device