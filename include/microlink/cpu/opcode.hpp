#pragma once

#include "microlink/cpu/types.hpp"

namespace microlink::cpu {

enum class Opcode : Byte {
    NOP   = 0x80,

    MOV   = 0x81,
    LOAD  = 0x82,
    STORE = 0x83,

    ADD   = 0x84,
    SUB   = 0x85,
    INC   = 0x86,
    DEC   = 0x87,

    AND   = 0x88,
    OR    = 0x89,
    XOR   = 0x8A,
    NOT   = 0x8B,

    CMP   = 0x8C,

    JMP   = 0x8D,
    JZ    = 0x8E,
    JNZ   = 0x8F,
    JC    = 0x90,
    JNC   = 0x91,
    CALL  = 0x92,
    RET   = 0x93,

    PUSH  = 0x94,
    POP   = 0x95,

    IN    = 0x96,
    OUT   = 0x97,

    HALT  = 0x98,
    EI    = 0x99,
    DI    = 0x9A,
    RETI  = 0x9B
};

constexpr Byte to_byte(Opcode opcode) noexcept {
    return static_cast<Byte>(opcode);
}

} // namespace microlink::cpu