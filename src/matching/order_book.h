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

    template <
        std::size_t OrderCapacity = 1'000'000, std::size_t LevelCapacity = 10'000,
        std::size_t EventCapacity = 262'144, std::size_t StopCapacity = 65'536>
    class OrderBook {
        using BidMap = std::pmr::map<core::Price, Level *, std::greater<core::Price>>;
        using AskMap = std::pmr::map<core::Price, Level *, std::less<core::Price>>;
        using StopMap = std::pmr::multimap<core::Price, Order *>;
        using OrderMap = std::pmr::unordered_map<core::OrderID, OrderRecord>;

    private:
        core::Symbol symbol_{};

        OwnedMonotonicBuffer order_index_buffer;
        OwnedMonotonicBuffer bid_index_buffer;
        OwnedMonotonicBuffer ask_index_buffer;
        OwnedMonotonicBuffer stop_buy_index_buffer;
        OwnedMonotonicBuffer stop_sell_index_buffer;

        static constexpr std::size_t ORDER_INDEX_BYTES = (OrderCapacity * 128U) + (OrderCapacity * 16U);
        static constexpr std::size_t LEVEL_INDEX_BYTES = LevelCapacity * 128U;
        static constexpr std::size_t STOP_INDEX_BYTES = StopCapacity * 128U;

        BidMap bids;
        AskMap asks;
        StopMap stop_buys;
        StopMap stop_sells;
        OrderMap orders;

        Level *best_bid{nullptr};
        Level *best_ask{nullptr};

        core::MemoryPool<Order, OrderCapacity> order_pool{};
        core::MemoryPool<Level, LevelCapacity> level_pool{};

        std::unique_ptr<Event[]> event_storage;
        std::size_t event_count{0};

        core::MatchID next_match_id{1};
        core::Price last_trade_price{0};

    private:
        void reset_events() noexcept { event_count = 0; }
        [[nodiscard]] std::span<const Event> events() const {
            return std::span<const Event>(event_storage.get(), event_count);
        }
    public:
        explicit OrderBook(core::Symbol symbol)
        : symbol_(symbol), order_index_buffer(ORDER_INDEX_BYTES),
          bid_index_buffer(LEVEL_INDEX_BYTES),
          ask_index_buffer(LEVEL_INDEX_BYTES),
          stop_buy_index_buffer(STOP_INDEX_BYTES),
          stop_sell_index_buffer(STOP_INDEX_BYTES),
          bids(std::greater<core::Price>{}, bid_index_buffer.resource()),
          asks(std::less<core::Price>{}, ask_index_buffer.resource()),
          stop_buys(stop_buy_index_buffer.resource()),
          stop_sells(stop_sell_index_buffer.resource()),
          orders(order_index_buffer.resource()),
          event_storage(std::make_unique<Event[]>(EventCapacity)) {
                orders.reserve(OrderCapacity);
            }

        OrderBook(const OrderBook &) = delete;
        OrderBook &operator=(const OrderBook &) = delete;
        OrderBook(OrderBook &&) = delete;
        OrderBook &operator=(OrderBook &&) = delete;

        std::span<const Event>

        AddOrder(core::OrderID order_id, core::Side side, core::Price price,
                  core::Quantity qty, core::OrderType order_type,
                  core::Timestamp timestamp, core::ParticipantID participant_id,
                  core::Price trigger_price = 0, core::Quantity display_qty = 0) {
            reset_events();
        }

    };
}
