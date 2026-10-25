#pragma once

#include <cstddef>
#include <type_traits>
#include "core/types.h"

namespace holt::exchange {

    struct Level;

    struct alignas(64) Order {
        core::OrderID id{0};
        core::Side side{core::Side::BUY};
        core::Price price{0};
        core::Quantity qty{0};
        core::Quantity original_qty{0};
        core::Quantity display_qty{0};
        core::Quantity hidden_qty{0};
        core::Timestamp timestamp{0};
        core::OrderType type{core::OrderType::LIMIT};
        core::OrderStatus status{core::OrderStatus::NEW};
        core::ParticipantID participant_id{0};
        core::Price trigger_price{0};
        core::Quantity peak_qty{0};
        Order *prev{nullptr};
        Order *next{nullptr};
        Level *parent_level{nullptr};
        [[nodiscard]] bool is_stop_order() const noexcept {
            return type == core::OrderType::STOP || type == core::OrderType::STOP_LIMIT;
        }
    };

    static_assert(sizeof(Order) <= 128,
        "Order should stay Compact enough for cache friendly access.");

    static_assert(std::is_standard_layout_v<Order>);

}