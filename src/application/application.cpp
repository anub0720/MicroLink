#include "microlink/application/application.hpp"

namespace microlink::application {

Application::Application() = default;

void Application::reset() noexcept {
    controller_.reset();
}

void Application::load_program(
    std::span<const cpu::Byte> program,
    cpu::Word start_address
) {
    controller_.load(
        program,
        start_address
    );
}

void Application::step() {
    controller_.step();
}

std::size_t Application::run(
    std::size_t cycle_limit
) {
    return controller_.run(cycle_limit);
}

void Application::pause() noexcept {
    controller_.pause();
}

runtime::RunState Application::state() const noexcept {
    return controller_.run_state();
}
const cpu::Memory&
Application::memory() const noexcept {
    return controller_.runtime().cpu().memory();
}
const cpu::CPUState&
Application::cpu_state() const noexcept {
    return controller_.runtime().cpu().state();
}

const std::vector<runtime::ExecutionTrace>&
Application::trace() const noexcept {
    return controller_.trace();
}

cpu::Byte Application::gpio() const noexcept {
    return controller_
        .runtime()
        .cpu()
        .devices()
        .gpio()
        .data();
}

cpu::Byte Application::gpio_direction() const noexcept {
    return controller_
        .runtime()
        .cpu()
        .devices()
        .gpio()
        .direction();
}

const std::string&
Application::uart_output() const noexcept {
    return controller_
        .runtime()
        .cpu()
        .devices()
        .uart()
        .output();
}

cpu::Byte Application::timer_counter() const noexcept {
    return controller_
        .runtime()
        .cpu()
        .devices()
        .timer()
        .counter();
}

cpu::Byte Application::timer_compare() const noexcept {
    return controller_
        .runtime()
        .cpu()
        .devices()
        .timer()
        .compare();
}

bool Application::timer_enabled() const noexcept {
    return controller_
        .runtime()
        .cpu()
        .devices()
        .timer()
        .enabled();
}

bool Application::timer_interrupt_pending() const noexcept {
    return controller_
        .runtime()
        .cpu()
        .devices()
        .timer()
        .interrupt_pending();
}

} // namespace microlink::application