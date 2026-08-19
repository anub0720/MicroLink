#include "microlink/cpu/cpu.hpp"

namespace microlink::cpu {

CPU::CPU() {
    memory_.attach_device_bus(
        &device_bus_
    );

    reset();
}

void CPU::reset() noexcept {
    state_.reset();
    memory_.reset();
    device_bus_.reset();
}

void CPU::load_program(
    std::span<const Byte> program,
    Word start_address
) {
    memory_.load(
        program,
        start_address
    );

    state_.set_program_counter(
        start_address
    );
}

Instruction CPU::fetch_and_decode() {
    const Word pc =
        state_.program_counter();

    const std::span<const Byte> remaining =
        memory_.view(pc);

    const DecodeResult result =
        Decoder::decode(remaining);

    state_.set_instruction_register(
        remaining[0]
    );

    return result.instruction;
}

Instruction CPU::cycle() {
    const Word instruction_address =
        state_.program_counter();

    const std::span<const Byte> remaining =
        memory_.view(
            instruction_address
        );

    const DecodeResult result =
        Decoder::decode(remaining);

    state_.set_instruction_register(
        remaining[0]
    );

    const Word next_program_counter =
        static_cast<Word>(
            instruction_address +
            static_cast<Word>(
                result.bytes_consumed
            )
        );

    state_.set_program_counter(
        next_program_counter
    );

    Executor::execute(
        result.instruction,
        state_,
        memory_,
        next_program_counter
    );

    device_bus_.tick();

    return result.instruction;
}

const CPUState&
CPU::state() const noexcept {
    return state_;
}

CPUState&
CPU::state() noexcept {
    return state_;
}

const Memory&
CPU::memory() const noexcept {
    return memory_;
}

Memory&
CPU::memory() noexcept {
    return memory_;
}

const device::DeviceBus&
CPU::devices() const noexcept {
    return device_bus_;
}

device::DeviceBus&
CPU::devices() noexcept {
    return device_bus_;
}

} // namespace microlink::cpu