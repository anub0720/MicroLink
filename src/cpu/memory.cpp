#include "microlink/cpu/memory.hpp"

#include <algorithm>
#include <cstddef>
#include <stdexcept>

#include "microlink/device/device_bus.hpp"

namespace microlink::cpu {

Memory::Memory() {
    reset();
}

void Memory::reset() noexcept {
    data_.fill(0);
}

Byte Memory::read(
    Word address
) const noexcept {
    if (
        device_bus_ != nullptr &&
        device_bus_->handles(address)
    ) {
        return device_bus_->read(address);
    }

    return data_[address];
}

void Memory::write(
    Word address,
    Byte value
) noexcept {
    if (
        device_bus_ != nullptr &&
        device_bus_->handles(address)
    ) {
        device_bus_->write(address, value);
        return;
    }

    data_[address] = value;
}

void Memory::load(
    std::span<const Byte> data,
    Word start_address
) {
    const std::size_t start =
        static_cast<std::size_t>(start_address);

    const std::size_t available =
        Size - start;

    if (data.size() > available) {
        throw std::runtime_error(
            "Program does not fit in memory"
        );
    }

    std::copy(
        data.begin(),
        data.end(),
        data_.begin() +
            static_cast<std::ptrdiff_t>(start)
    );
}

std::span<const Byte> Memory::view(
    Word start_address
) const noexcept {
    const std::size_t address =
        static_cast<std::size_t>(start_address);

    return std::span<const Byte>(
        data_.data() + address,
        Size - address
    );
}

void Memory::attach_device_bus(
    device::DeviceBus* bus
) noexcept {
    device_bus_ = bus;
}

} // namespace microlink::cpu