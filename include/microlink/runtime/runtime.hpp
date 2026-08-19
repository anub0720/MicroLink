#pragma once

#include <cstddef>
#include <span>
#include <vector>

#include "microlink/cpu/cpu.hpp"
#include "microlink/cpu/types.hpp"
#include "microlink/runtime/trace.hpp"

namespace microlink::runtime {

class Runtime {
public:
    Runtime();

    void reset() noexcept;

    void load_program(
        std::span<const cpu::Byte> program,
        cpu::Word start_address
    );

    void step();

    std::size_t run(std::size_t max_cycles);

    bool halted() const noexcept;

    std::size_t cycles_executed() const noexcept;

    const cpu::CPU& cpu() const noexcept;
    cpu::CPU& cpu() noexcept;

    const std::vector<ExecutionTrace>& trace() const noexcept;

    void clear_trace();

private:
    cpu::CPU cpu_{};
    std::size_t cycles_executed_{0};
    std::vector<ExecutionTrace> trace_{};
};

} // namespace microlink::runtime