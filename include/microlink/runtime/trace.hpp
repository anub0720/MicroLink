#pragma once

#include <cstddef>

#include "microlink/cpu/instruction.hpp"
#include "microlink/cpu/types.hpp"

namespace microlink::runtime {

struct ExecutionTrace {
    std::size_t cycle{0};

    cpu::Word program_counter_before{0};
    cpu::Word program_counter_after{0};

    cpu::Instruction instruction{
        cpu::Opcode::NOP
    };

    cpu::Byte accumulator_before{0};
    cpu::Byte accumulator_after{0};

    cpu::Word stack_pointer_before{0};
    cpu::Word stack_pointer_after{0};
};

} // namespace microlink::runtime