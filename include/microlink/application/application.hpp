#pragma once

#include <cstddef>
#include <span>
#include <string>
#include <vector>

#include "microlink/cpu/cpu_state.hpp"
#include "microlink/cpu/types.hpp"
#include "microlink/runtime/controller.hpp"
#include "microlink/runtime/trace.hpp"

namespace microlink::application {

class Application {
public:
    Application();

    void reset() noexcept;

    void load_program(
        std::span<const cpu::Byte> program,
        cpu::Word start_address
    );

    void step();

    std::size_t run(std::size_t cycle_limit);
    const cpu::Memory& memory() const noexcept;
    void pause() noexcept;

    runtime::RunState state() const noexcept;

    const cpu::CPUState& cpu_state() const noexcept;

    const std::vector<runtime::ExecutionTrace>&
    trace() const noexcept;

    cpu::Byte gpio() const noexcept;
    cpu::Byte gpio_direction() const noexcept;

    const std::string& uart_output() const noexcept;

    cpu::Byte timer_counter() const noexcept;
    cpu::Byte timer_compare() const noexcept;
    bool timer_enabled() const noexcept;
    bool timer_interrupt_pending() const noexcept;

private:
    runtime::Controller controller_{};
};

} // namespace microlink::application