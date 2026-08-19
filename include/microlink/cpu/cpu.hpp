#pragma once

#include <span>

#include "microlink/cpu/cpu_state.hpp"
#include "microlink/cpu/decoder.hpp"
#include "microlink/cpu/executor.hpp"
#include "microlink/cpu/instruction.hpp"
#include "microlink/cpu/memory.hpp"
#include "microlink/cpu/types.hpp"
#include "microlink/device/device_bus.hpp"

namespace microlink::cpu {

class CPU {
public:
    CPU();

    void reset() noexcept;

    void load_program(
        std::span<const Byte> program,
        Word start_address
    );

    Instruction fetch_and_decode();

    Instruction cycle();

    const CPUState& state() const noexcept;
    CPUState& state() noexcept;

    const Memory& memory() const noexcept;
    Memory& memory() noexcept;

    const device::DeviceBus&
    devices() const noexcept;

    device::DeviceBus&
    devices() noexcept;

private:
    CPUState state_{};

    Memory memory_{};

    device::DeviceBus device_bus_{};
};

} // namespace microlink::cpu