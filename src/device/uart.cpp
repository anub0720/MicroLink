#include "microlink/device/uart.hpp"

namespace microlink::device {

UART::UART() {
    reset();
}

void UART::reset() noexcept {
    output_.clear();
}

bool UART::handles(
    cpu::Word address
) const noexcept {
    return address == DataAddress ||
           address == StatusAddress;
}

cpu::Byte UART::read(
    cpu::Word address
) noexcept {
    if (address == StatusAddress) {
        return 0x01;
    }

    return 0;
}

void UART::write(
    cpu::Word address,
    cpu::Byte value
) noexcept {
    if (address == DataAddress) {
        output_.push_back(
            static_cast<char>(value)
        );
    }
}

void UART::tick() noexcept {
}

const std::string&
UART::output() const noexcept {
    return output_;
}

void UART::clear_output() {
    output_.clear();
}

} // namespace microlink::device