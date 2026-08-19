#pragma once

#include <array>
#include <cstddef>
#include <span>

#include "microlink/cpu/types.hpp"

namespace microlink::device {
class DeviceBus;
}

namespace microlink::cpu {

class Memory {
public:
    static constexpr std::size_t Size = 65536;

    Memory();

    void reset() noexcept;

    Byte read(Word address) const noexcept;

    void write(
        Word address,
        Byte value
    ) noexcept;

    void load(
        std::span<const Byte> data,
        Word start_address
    );

    std::span<const Byte> view(
        Word start_address
    ) const noexcept;

    void attach_device_bus(
        device::DeviceBus* bus
    ) noexcept;

private:
    std::array<Byte, Size> data_{};

    device::DeviceBus* device_bus_{nullptr};
};

} // namespace microlink::cpu