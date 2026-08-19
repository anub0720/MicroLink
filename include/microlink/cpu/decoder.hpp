#pragma once

#include <cstddef>
#include <span>
#include <string>

#include "microlink/cpu/instruction.hpp"

namespace microlink::cpu {

struct DecodeResult {
    Instruction instruction;
    std::size_t bytes_consumed;
};

class Decoder {
public:
    static DecodeResult decode(std::span<const Byte> bytes);
};

} // namespace microlink::cpu