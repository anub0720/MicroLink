#pragma once

#include <cstddef>
#include <span>
#include <vector>

#include "microlink/cpu/cpu_state.hpp"
#include "microlink/cpu/types.hpp"
#include "microlink/runtime/runtime.hpp"
#include "microlink/runtime/trace.hpp"

namespace microlink::runtime {

enum class RunState {
    Ready,
    Running,
    Paused,
    Halted
};

class Controller {
public:
    Controller();

    void reset() noexcept;

    void load(
        std::span<const cpu::Byte> program,
        cpu::Word start_address
    );

    void step();

   std::size_t run(std::size_t cycle_limit);

    void pause() noexcept;

    RunState run_state() const noexcept;

    const Runtime& runtime() const noexcept;
    Runtime& runtime() noexcept;

    const std::vector<ExecutionTrace>& trace() const noexcept;

private:
    Runtime runtime_{};
    RunState state_{RunState::Ready};
};

} // namespace microlink::runtime