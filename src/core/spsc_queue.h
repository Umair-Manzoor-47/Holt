#pragma once

#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace holt::core {
    template <typename T, std::size_t N>
    class SPSCQueue {
        static_assert(N>1, "SPSCQueue requires at least 2 empty slots.");
        static_assert(( N & (N - 1U)) == 0U,
            "SPSCQueue capacity must be a power of two.");
        static_assert(std::is_trivially_copyable_v<T>,
            "SPSCQueue payload must be trivially copyable.");

        struct alignas(128) Cursor {
            std::atomic<std::uint64_t> value{0};
        };
    private:
        static constexpr std::size_t MASK = N - 1U;

        Cursor m_Head{};
        Cursor m_Tail{};
        std::array<T, N> m_Buffer{};
    public:
        bool push(const T &value) noexcept {
            const std::uint64_t tail = m_Tail.value.load(std::memory_order_relaxed);
            const std::uint64_t head = m_Head.value.load(std::memory_order_acquire);
            if ((tail - head) == N) [[unlikely]] {
                return false;
            }

            m_Buffer[m_Tail & MASK] = value;
            m_Tail.value.store(tail + 1U, std::memory_order_release);
            return true;
        }

       bool pop(const T &value) noexcept {
            const std::uint64_t head = m_Head.value.load(std::memory_order_relaxed);
            const std::uint64_t tail = m_Tail.value.load(std::memory_order_acquire);
            if (head == tail) [[unlikely]] {
                return false;
            }

            value = m_Buffer[m_Head];
            m_Head.value.store(head + 1U, std::memory_order_release);
            return true;
        }

        [[nodiscard]] bool empty() const noexcept {
            return m_Head.value.load(std::memory_order_acquire) ==
                   m_Tail.value.load(std::memory_order_acquire);
        }
    };
}