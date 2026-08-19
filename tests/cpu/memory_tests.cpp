#include <array>
#include <gtest/gtest.h>

#include "microlink/cpu/memory.hpp"

using namespace microlink::cpu;

TEST(Memory, StartsCleared) {
    Memory memory;

    EXPECT_EQ(memory.read(0x0000), 0);
    EXPECT_EQ(memory.read(0x2040), 0);
    EXPECT_EQ(memory.read(0xFFFF), 0);
}

TEST(Memory, CanWriteAndReadByte) {
    Memory memory;

    memory.write(0x2040, 0xAB);

    EXPECT_EQ(memory.read(0x2040), 0xAB);
}

TEST(Memory, DifferentAddressesAreIndependent) {
    Memory memory;

    memory.write(0x1000, 0x12);
    memory.write(0x1001, 0x34);

    EXPECT_EQ(memory.read(0x1000), 0x12);
    EXPECT_EQ(memory.read(0x1001), 0x34);
}

TEST(Memory, CanLoadDataAtAddress) {
    Memory memory;

    const std::array<Byte, 4> data{
        0x10,
        0x20,
        0x30,
        0x40
    };

    memory.load(data, 0x2000);

    EXPECT_EQ(memory.read(0x2000), 0x10);
    EXPECT_EQ(memory.read(0x2001), 0x20);
    EXPECT_EQ(memory.read(0x2002), 0x30);
    EXPECT_EQ(memory.read(0x2003), 0x40);
}

TEST(Memory, ResetClearsPreviouslyWrittenData) {
    Memory memory;

    memory.write(0x2040, 0xAB);

    memory.reset();

    EXPECT_EQ(memory.read(0x2040), 0);
}

TEST(Memory, CanUseLastAddress) {
    Memory memory;

    memory.write(0xFFFF, 0x5A);

    EXPECT_EQ(memory.read(0xFFFF), 0x5A);
}

TEST(Memory, RejectsDataThatDoesNotFit) {
    Memory memory;

    const std::array<Byte, 2> data{
        0xAA,
        0xBB
    };

    EXPECT_THROW(
        memory.load(data, 0xFFFF),
        std::runtime_error
    );
}