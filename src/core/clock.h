#pragma once

#include <chrono>
#include "core/types.h"

namespace holt::core {
    class LogicalClock {
    private:
        Timestamp m_Current{0};

    public:
        [[nodiscard]] Timestamp next() noexcept { return ++m_Current; }
        [[nodiscard]] Timestamp current() const noexcept { return m_Current; }

        void reset() noexcept { m_Current = 0; }
    };
    [[nodiscard]] inline Timestamp steady_timestamp_now() noexcept {
        const auto now = std::chrono::steady_clock::now().time_since_epoch();
        return static_cast<Timestamp>(
        std::chrono::duration_cast<std::chrono::nanoseconds>(now).count()
        );
    }
    [[nodiscard]] inline Timestamp now_ns() noexcept {
        return steady_timestamp_now();
    }
}