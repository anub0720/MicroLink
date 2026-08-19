#include "microlink/device/timer.hpp"

namespace microlink::device {

Timer::Timer() {
    reset();
}

void Timer::reset() noexcept {
    counter_ = 0;
    compare_ = 0;
    control_ = 0;
    interrupt_pending_ = false;
}

bool Timer::handles(
    cpu::Word address
) const noexcept {
    return address == CounterAddress ||
           address == CompareAddress ||
           address == ControlAddress;
}

cpu::Byte Timer::read(
    cpu::Word address
) noexcept {
    if (address == CounterAddress) {
        return counter_;
    }

    if (address == CompareAddress) {
        return compare_;
    }

    if (address == ControlAddress) {
        return control_;
    }

    return 0;
}

void Timer::write(
    cpu::Word address,
    cpu::Byte value
) noexcept {
    if (address == CounterAddress) {
        counter_ = value;
        interrupt_pending_ = false;
        return;
    }

    if (address == CompareAddress) {
        compare_ = value;
        return;
    }

    if (address == ControlAddress) {
        control_ = value;

        if ((control_ & 0x02U) != 0U) {
            interrupt_pending_ = false;
        }
    }
}

void Timer::tick() noexcept {
    if (!enabled()) {
        return;
    }

    ++counter_;

    if (counter_ == compare_) {
        interrupt_pending_ = true;
    }
}

cpu::Byte Timer::counter() const noexcept {
    return counter_;
}

cpu::Byte Timer::compare() const noexcept {
    return compare_;
}

bool Timer::enabled() const noexcept {
    return (control_ & 0x01U) != 0U;
}

bool Timer::interrupt_pending() const noexcept {
    return interrupt_pending_;
}

} // namespace microlink::device