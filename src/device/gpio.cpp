#include "microlink/device/gpio.hpp"

namespace microlink::device {

GPIO::GPIO() {
    reset();
}

void GPIO::reset() noexcept {
    data_ = 0;
    direction_ = 0;
}

bool GPIO::handles(
    cpu::Word address
) const noexcept {
    return address == DataAddress ||
           address == DirectionAddress;
}

cpu::Byte GPIO::read(
    cpu::Word address
) noexcept {
    if (address == DataAddress) {
        return data_;
    }

    if (address == DirectionAddress) {
        return direction_;
    }

    return 0;
}

void GPIO::write(
    cpu::Word address,
    cpu::Byte value
) noexcept {
    if (address == DataAddress) {
        data_ = value;
        return;
    }

    if (address == DirectionAddress) {
        direction_ = value;
    }
}

void GPIO::tick() noexcept {
}

cpu::Byte GPIO::data() const noexcept {
    return data_;
}

cpu::Byte GPIO::direction() const noexcept {
    return direction_;
}

} // namespace microlink::device