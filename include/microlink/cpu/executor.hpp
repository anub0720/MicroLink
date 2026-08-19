#pragma once

#include "microlink/cpu/cpu_state.hpp"
#include "microlink/cpu/instruction.hpp"
#include "microlink/cpu/memory.hpp"
#include "microlink/cpu/types.hpp"

namespace microlink::cpu {

class Executor {
public:
    static void execute(
        const Instruction& instruction,
        CPUState& state,
        Memory& memory,
        Word next_program_counter
    );
};

} // namespace microlink::cpu