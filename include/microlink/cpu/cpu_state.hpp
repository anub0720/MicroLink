#pragma once

#include <array>
#include <cstddef>

#include "microlink/cpu/flags.hpp"
#include "microlink/cpu/types.hpp"

namespace microlink::cpu {

class CPUState {
public:
    static constexpr std::size_t GeneralRegisterCount = 6;

    void reset() noexcept;

    Byte accumulator() const noexcept;
    Byte register_value(std::size_t index) const noexcept;

    Word program_counter() const noexcept;
    Word stack_pointer() const noexcept;
    Byte instruction_register() const noexcept;

    bool halted() const noexcept;
    bool interrupts_enabled() const noexcept;

    const Flags& flags() const noexcept;
    Flags& flags() noexcept;

    void set_accumulator(Byte value) noexcept;
    void set_register(std::size_t index, Byte value);
    void set_program_counter(Word value) noexcept;
    void set_stack_pointer(Word value) noexcept;
    void set_instruction_register(Byte value) noexcept;

    void set_halted(bool value) noexcept;
    void set_interrupts_enabled(bool value) noexcept;

private:
    Byte accumulator_{0};
    std::array<Byte, GeneralRegisterCount> registers_{};

    Word program_counter_{0};
    Word stack_pointer_{0};

    Byte instruction_register_{0};

    bool halted_{false};
    bool interrupts_enabled_{false};

    Flags flags_{};
};

} // namespace microlink::cpu