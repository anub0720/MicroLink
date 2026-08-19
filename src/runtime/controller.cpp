#include "microlink/runtime/controller.hpp"

namespace microlink::runtime {

Controller::Controller() = default;

void Controller::reset() noexcept {
    runtime_.reset();
    state_ = RunState::Ready;
}

void Controller::load(
    std::span<const cpu::Byte> program,
    cpu::Word start_address
) {
    runtime_.load_program(program, start_address);
    state_ = RunState::Ready;
}

void Controller::step() {
    if (runtime_.halted()) {
        state_ = RunState::Halted;
        return;
    }

    runtime_.step();

    if (runtime_.halted()) {
        state_ = RunState::Halted;
    } else {
        state_ = RunState::Paused;
    }
}

std::size_t Controller::run(
    std::size_t max_cycles
) {
    if (runtime_.halted()) {
        state_ = RunState::Halted;
        return 0;
    }

    state_ = RunState::Running;

    const std::size_t executed =
        runtime_.run(max_cycles);

    if (runtime_.halted()) {
        state_ = RunState::Halted;
    } else {
        state_ = RunState::Paused;
    }

    return executed;
}

void Controller::pause() noexcept {
    if (!runtime_.halted()) {
        state_ = RunState::Paused;
    }
}

RunState Controller::run_state() const noexcept {
    return state_;
}

const Runtime& Controller::runtime() const noexcept {
    return runtime_;
}

Runtime& Controller::runtime() noexcept {
    return runtime_;
}

const std::vector<ExecutionTrace>&
Controller::trace() const noexcept {
    return runtime_.trace();
}

} // namespace microlink::runtime