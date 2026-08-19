#include "microlink/cpu/cpu_state.hpp"

#include <stdexcept>

namespace microlink::cpu {

void CPUState::reset() noexcept {
    accumulator_ = 0;
    registers_.fill(0);

    program_counter_ = 0;
    stack_pointer_ = 0;

    instruction_register_ = 0;

    halted_ = false;
    interrupts_enabled_ = false;

    flags_.reset();
}

Byte CPUState::accumulator() const noexcept {
    return accumulator_;
}

Byte CPUState::register_value(std::size_t index) const noexcept {
    if (index >= GeneralRegisterCount) {
        return 0;
    }

    return registers_[index];
}

Word CPUState::program_counter() const noexcept {
    return program_counter_;
}

Word CPUState::stack_pointer() const noexcept {
    return stack_pointer_;
}

Byte CPUState::instruction_register() const noexcept {
    return instruction_register_;
}

bool CPUState::halted() const noexcept {
    return halted_;
}

bool CPUState::interrupts_enabled() const noexcept {
    return interrupts_enabled_;
}

const Flags& CPUState::flags() const noexcept {
    return flags_;
}

Flags& CPUState::flags() noexcept {
    return flags_;
}

void CPUState::set_accumulator(Byte value) noexcept {
    accumulator_ = value;
}

void CPUState::set_register(std::size_t index, Byte value) {
    if (index >= GeneralRegisterCount) {
        throw std::out_of_range("CPU register index out of range");
    }

    registers_[index] = value;
}

void CPUState::set_program_counter(Word value) noexcept {
    program_counter_ = value;
}

void CPUState::set_stack_pointer(Word value) noexcept {
    stack_pointer_ = value;
}

void CPUState::set_instruction_register(Byte value) noexcept {
    instruction_register_ = value;
}

void CPUState::set_halted(bool value) noexcept {
    halted_ = value;
}

void CPUState::set_interrupts_enabled(bool value) noexcept {
    interrupts_enabled_ = value;
}

} // namespace microlink::cpu