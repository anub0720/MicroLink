#include "microlink/cpu/executor.hpp"

#include <stdexcept>

#include "microlink/cpu/alu.hpp"

namespace microlink::cpu
{

    namespace
    {

        Byte read_register(
            const CPUState &state,
            RegisterId register_id)
        {
            switch (register_id)
            {
            case RegisterId::A:
                return state.accumulator();
            case RegisterId::R0:
                return state.register_value(0);
            case RegisterId::R1:
                return state.register_value(1);
            case RegisterId::R2:
                return state.register_value(2);
            case RegisterId::R3:
                return state.register_value(3);
            case RegisterId::R4:
                return state.register_value(4);
            case RegisterId::R5:
                return state.register_value(5);
            }

            throw std::runtime_error("Invalid register");
        }

        void write_register(
            CPUState &state,
            RegisterId register_id,
            Byte value)
        {
            switch (register_id)
            {
            case RegisterId::A:
                state.set_accumulator(value);
                return;
            case RegisterId::R0:
                state.set_register(0, value);
                return;
            case RegisterId::R1:
                state.set_register(1, value);
                return;
            case RegisterId::R2:
                state.set_register(2, value);
                return;
            case RegisterId::R3:
                state.set_register(3, value);
                return;
            case RegisterId::R4:
                state.set_register(4, value);
                return;
            case RegisterId::R5:
                state.set_register(5, value);
                return;
            }

            throw std::runtime_error("Invalid register");
        }

        Byte read_value(
            const Operand &operand,
            const CPUState &state)
        {
            switch (operand.type)
            {
            case OperandType::Register:
                return read_register(state, operand.register_id());

            case OperandType::Immediate:
                return static_cast<Byte>(operand.value);

            default:
                throw std::runtime_error(
                    "Operand cannot be used as a byte value");
            }
        }

        ALUOperation alu_operation_for(Opcode opcode)
        {
            switch (opcode)
            {
            case Opcode::ADD:
                return ALUOperation::Add;
            case Opcode::SUB:
                return ALUOperation::Subtract;
            case Opcode::AND:
                return ALUOperation::And;
            case Opcode::OR:
                return ALUOperation::Or;
            case Opcode::XOR:
                return ALUOperation::Xor;
            case Opcode::CMP:
                return ALUOperation::Compare;
            default:
                throw std::runtime_error(
                    "Opcode is not an ALU operation");
            }
        }

        void execute_alu_instruction(
            const Instruction &instruction,
            CPUState &state)
        {
            const Operand &operand = instruction.operand(0);

            if (operand.type != OperandType::Register)
            {
                throw std::runtime_error(
                    "ALU operand must be a register");
            }

            const ALUResult result = ALU::execute(
                alu_operation_for(instruction.opcode()),
                state.accumulator(),
                read_register(state, operand.register_id()),
                state.flags());

            if (instruction.opcode() != Opcode::CMP)
            {
                state.set_accumulator(result.value);
            }

            state.flags() = result.flags;
        }

        void execute_inc(
            const Instruction &instruction,
            CPUState &state)
        {
            const Operand &operand = instruction.operand(0);

            if (operand.type != OperandType::Register)
            {
                throw std::runtime_error("INC operand must be a register");
            }

            const ALUResult result = ALU::execute(
                ALUOperation::Add,
                read_register(state, operand.register_id()),
                1,
                state.flags());

            write_register(
                state,
                operand.register_id(),
                result.value);

            state.flags() = result.flags;
        }

        void execute_dec(
            const Instruction &instruction,
            CPUState &state)
        {
            const Operand &operand = instruction.operand(0);

            if (operand.type != OperandType::Register)
            {
                throw std::runtime_error("DEC operand must be a register");
            }

            const ALUResult result = ALU::execute(
                ALUOperation::Subtract,
                read_register(state, operand.register_id()),
                1,
                state.flags());

            write_register(
                state,
                operand.register_id(),
                result.value);

            state.flags() = result.flags;
        }

        void execute_not(
            const Instruction &instruction,
            CPUState &state)
        {
            const Operand &operand = instruction.operand(0);

            if (operand.type != OperandType::Register)
            {
                throw std::runtime_error("NOT operand must be a register");
            }

            const ALUResult result = ALU::execute(
                ALUOperation::Xor,
                read_register(state, operand.register_id()),
                0xFF,
                state.flags());

            write_register(
                state,
                operand.register_id(),
                result.value);

            state.flags() = result.flags;
        }

        void execute_load(
            const Instruction &instruction,
            CPUState &state,
            const Memory &memory)
        {
            const Operand &destination = instruction.operand(0);
            const Operand &address = instruction.operand(1);

            if (destination.type != OperandType::Register ||
                address.type != OperandType::Address)
            {
                throw std::runtime_error("Invalid LOAD operands");
            }

            write_register(
                state,
                destination.register_id(),
                memory.read(static_cast<Word>(address.value)));
        }

        void execute_store(
            const Instruction &instruction,
            const CPUState &state,
            Memory &memory)
        {
            const Operand &source = instruction.operand(0);
            const Operand &address = instruction.operand(1);

            if (source.type != OperandType::Register ||
                address.type != OperandType::Address)
            {
                throw std::runtime_error("Invalid STORE operands");
            }

            memory.write(
                static_cast<Word>(address.value),
                read_register(state, source.register_id()));
        }

        bool branch_condition(
            Opcode opcode,
            const Flags &flags)
        {
            switch (opcode)
            {
            case Opcode::JMP:
                return true;
            case Opcode::JZ:
                return flags.zero();
            case Opcode::JNZ:
                return !flags.zero();
            case Opcode::JC:
                return flags.carry();
            case Opcode::JNC:
                return !flags.carry();
            default:
                throw std::runtime_error("Invalid branch opcode");
            }
        }

        void execute_branch(
            const Instruction &instruction,
            CPUState &state)
        {
            const Operand &target = instruction.operand(0);

            if (target.type != OperandType::Address)
            {
                throw std::runtime_error(
                    "Branch target must be an address");
            }

            if (branch_condition(instruction.opcode(), state.flags()))
            {
                state.set_program_counter(
                    static_cast<Word>(target.value));
            }
        }

        void push_byte(
            Memory &memory,
            CPUState &state,
            Byte value)
        {
            const Word new_sp =
                static_cast<Word>(state.stack_pointer() - 1U);

            state.set_stack_pointer(new_sp);
            memory.write(new_sp, value);
        }

        Byte pop_byte(
            Memory &memory,
            CPUState &state)
        {
            const Word sp = state.stack_pointer();
            const Byte value = memory.read(sp);

            state.set_stack_pointer(
                static_cast<Word>(sp + 1U));

            return value;
        }

        void push_word(
            Memory &memory,
            CPUState &state,
            Word value)
        {
            const Byte high =
                static_cast<Byte>((value >> 8U) & 0xFFU);

            const Byte low =
                static_cast<Byte>(value & 0xFFU);

            push_byte(memory, state, high);
            push_byte(memory, state, low);
        }

        Word pop_word(
            Memory &memory,
            CPUState &state)
        {
            const Byte low = pop_byte(memory, state);
            const Byte high = pop_byte(memory, state);

            return static_cast<Word>(
                (static_cast<Word>(high) << 8U) |
                static_cast<Word>(low));
        }

        void execute_push(
            const Instruction &instruction,
            CPUState &state,
            Memory &memory)
        {
            const Operand &operand = instruction.operand(0);

            if (operand.type != OperandType::Register)
            {
                throw std::runtime_error(
                    "PUSH operand must be a register");
            }

            push_byte(
                memory,
                state,
                read_register(state, operand.register_id()));
        }

        void execute_pop(
            const Instruction &instruction,
            CPUState &state,
            Memory &memory)
        {
            const Operand &operand = instruction.operand(0);

            if (operand.type != OperandType::Register)
            {
                throw std::runtime_error(
                    "POP operand must be a register");
            }

            write_register(
                state,
                operand.register_id(),
                pop_byte(memory, state));
        }
        void execute_in(
            const Instruction &instruction,
            CPUState &state,
            Memory &memory)
        {
            const Operand &address =
                instruction.operand(0);

            if (address.type != OperandType::Address)
            {
                throw std::runtime_error(
                    "IN operand must be an address");
            }

            state.set_accumulator(
                memory.read(
                    static_cast<Word>(address.value)));
        }

        void execute_out(
            const Instruction &instruction,
            CPUState &state,
            Memory &memory)
        {
            const Operand &address =
                instruction.operand(0);

            if (address.type != OperandType::Address)
            {
                throw std::runtime_error(
                    "OUT operand must be an address");
            }

            memory.write(
                static_cast<Word>(address.value),
                state.accumulator());
        }
        void execute_call(
            const Instruction &instruction,
            CPUState &state,
            Memory &memory,
            Word next_program_counter)
        {
            const Operand &target = instruction.operand(0);

            if (target.type != OperandType::Address)
            {
                throw std::runtime_error(
                    "CALL target must be an address");
            }

            push_word(
                memory,
                state,
                next_program_counter);

            state.set_program_counter(
                static_cast<Word>(target.value));
        }

        void execute_ret(
            CPUState &state,
            Memory &memory)
        {
            state.set_program_counter(
                pop_word(memory, state));
        }

    } // namespace

    void Executor::execute(
        const Instruction &instruction,
        CPUState &state,
        Memory &memory,
        Word next_program_counter)
    {
        switch (instruction.opcode())
        {

        case Opcode::MOV:
        {
            const Operand &destination = instruction.operand(0);
            const Operand &source = instruction.operand(1);

            if (destination.type != OperandType::Register)
            {
                throw std::runtime_error(
                    "MOV destination must be a register");
            }

            write_register(
                state,
                destination.register_id(),
                read_value(source, state));

            return;
        }

        case Opcode::LOAD:
            execute_load(instruction, state, memory);
            return;

        case Opcode::STORE:
            execute_store(instruction, state, memory);
            return;

        case Opcode::ADD:
        case Opcode::SUB:
        case Opcode::AND:
        case Opcode::OR:
        case Opcode::XOR:
        case Opcode::CMP:
            execute_alu_instruction(instruction, state);
            return;

        case Opcode::INC:
            execute_inc(instruction, state);
            return;

        case Opcode::DEC:
            execute_dec(instruction, state);
            return;

        case Opcode::NOT:
            execute_not(instruction, state);
            return;

        case Opcode::JMP:
        case Opcode::JZ:
        case Opcode::JNZ:
        case Opcode::JC:
        case Opcode::JNC:
            execute_branch(instruction, state);
            return;

        case Opcode::PUSH:
            execute_push(instruction, state, memory);
            return;

        case Opcode::POP:
            execute_pop(instruction, state, memory);
            return;

        case Opcode::CALL:
            execute_call(
                instruction,
                state,
                memory,
                next_program_counter);
            return;

        case Opcode::RET:
            execute_ret(state, memory);
            return;
        case Opcode::IN:
            execute_in(
                instruction,
                state,
                memory);
            return;

        case Opcode::OUT:
            execute_out(
                instruction,
                state,
                memory);
            return;
        case Opcode::NOP:
            return;
        case Opcode::HALT:
            state.set_halted(true);
            return;
        default:
            throw std::runtime_error(
                "Instruction execution not implemented");
        }
    }

} // namespace microlink::cpu