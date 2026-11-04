#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <limits>
#include <map>
#include <memory>
#include <memory_resource>
#include <optional>
#include <span>
#include <unordered_map>

#include "core/memory_pool.h"
#include "core/types.h"
#include "matching/events.h"
#include "matching/level.h"

namespace holt::matching {
    struct OrderRecord {
        Order *order{nullptr};
        core::OrderStatus last_status{core::OrderStatus::NEW};
    };
    static_assert(std::is_trivially_copyable_v<OrderRecord>);

    struct PriceLevelView {
        core::Price price{0};
        core::Quantity qty{0};
    };

    static_assert(std::is_trivially_copyable_v<PriceLevelView>);

    class OwnedMonotonicBuffer {
    public:
        explicit OwnedMonotonicBuffer(std::size_t bytes)
        : storage_(std::make_unique<std::byte[]>(bytes)),
          resource_(storage_.get(), bytes, std::pmr::null_memory_resource()) {}

        OwnedMonotonicBuffer(const OwnedMonotonicBuffer &) = delete;
        OwnedMonotonicBuffer &operator=(const OwnedMonotonicBuffer &) = delete;
        OwnedMonotonicBuffer(OwnedMonotonicBuffer &&) = delete;
        OwnedMonotonicBuffer &operator=(OwnedMonotonicBuffer &&) = delete;
        [[nodiscard]] std::pmr::memory_resource *resource() noexcept { return &resource_;  }

    private:
        std::unique_ptr<std::byte[]> storage_;
        std::pmr::monotonic_buffer_resource resource_;
    };
}
