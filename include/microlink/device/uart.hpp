#pragma once

#include <string>

#include "microlink/device/device.hpp"

namespace microlink::device {

class UART final : public Device {
public:
    static constexpr cpu::Word DataAddress = 0xF010;
    static constexpr cpu::Word StatusAddress = 0xF011;

    UART();

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

    const std::string& output() const noexcept;

    void clear_output();

private:
    std::string output_{};
};

} // namespace microlink::device