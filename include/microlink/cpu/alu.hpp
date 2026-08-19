#pragma once

#include "microlink/cpu/flags.hpp"
#include "microlink/cpu/types.hpp"

namespace microlink::cpu {

enum class ALUOperation {
    Add,
    Subtract,
    And,
    Or,
    Xor,
    Compare
};

struct ALUResult {
    Byte value;
    Flags flags;
};

class ALU {
public:
    static ALUResult execute(
        ALUOperation operation,
        Byte lhs,
        Byte rhs,
        const Flags& previous_flags
    );
};

} // namespace microlink::cpu