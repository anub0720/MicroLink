#include "microlink/runtime/runtime.hpp"

namespace microlink::runtime {

Runtime::Runtime() = default;

void Runtime::reset() noexcept {
    cpu_.reset();
    cycles_executed_ = 0;
    trace_.clear();
}

void Runtime::load_program(
    std::span<const cpu::Byte> program,
    cpu::Word start_address
) {
    cpu_.load_program(program, start_address);
    cycles_executed_ = 0;
    trace_.clear();
}

void Runtime::step() {
    if (cpu_.state().halted()) {
        return;
    }

    const cpu::CPUState& before =
        cpu_.state();

    ExecutionTrace event;

    event.cycle = cycles_executed_ + 1;
    event.program_counter_before =
        before.program_counter();
    event.accumulator_before =
        before.accumulator();
    event.stack_pointer_before =
        before.stack_pointer();

    event.instruction =
        cpu_.cycle();

    const cpu::CPUState& after =
        cpu_.state();

    event.program_counter_after =
        after.program_counter();
    event.accumulator_after =
        after.accumulator();
    event.stack_pointer_after =
        after.stack_pointer();

    ++cycles_executed_;

    trace_.push_back(event);
}

std::size_t Runtime::run(
    std::size_t max_cycles
) {
    std::size_t executed = 0;

    while (
        executed < max_cycles &&
        !cpu_.state().halted()
    ) {
        step();
        ++executed;
    }

    return executed;
}

bool Runtime::halted() const noexcept {
    return cpu_.state().halted();
}

std::size_t Runtime::cycles_executed() const noexcept {
    return cycles_executed_;
}

const cpu::CPU& Runtime::cpu() const noexcept {
    return cpu_;
}

cpu::CPU& Runtime::cpu() noexcept {
    return cpu_;
}

const std::vector<ExecutionTrace>&
Runtime::trace() const noexcept {
    return trace_;
}

void Runtime::clear_trace() {
    trace_.clear();
}

} // namespace microlink::runtime