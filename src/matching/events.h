#pragma once

#include <cstdint>
#include <type_traits>

#include "core/types.h"

namespace holt::matching {
    using core::MatchID;
    using core::OrderID;
    using core::OrderType;
    using core::Price;
    using core::Quantity;
    using core::SequenceNumber;
    using core::Side;
    using core::Symbol;
    using core::Timestamp;

    enum class ReasonCode: std::uint16_t {
        NONE = 0,
        ZERO_QUANTITY,
        NEGATIVE_PRICE,
        BOOK_EMPTY,
        DUPLICATE_ORDER_ID,
        ORDER_NOT_FOUND,
        ORDER_NOT_ACTIVE,
        FOK_INSUFFICIENT_LIQUIDITY,
        POST_ONLY_WOULD_CROSS,
        INVALID_MODIFICATION,
        INVALID_ICEBERG_DISPLAY,
        ORDER_POOL_EXHAUSTED
    };

    enum class EventType: std::uint8_t {
        ORDER_ACCEPTED,
        ORDER_RESTED,
        ORDER_REJECTED,
        ORDER_REDUCED,
        ORDER_PARTIALLY_FILLED,
        ORDER_FILLED,
        ORDER_CANCELLED,
        TRADE
    };

    struct OrderAccepted {
        SequenceNumber sequence_number;
        Timestamp timestamp;
        OrderID order_id;
        Symbol symbol;
        Side side;
        Price price;
        Quantity qty;
        OrderType order_type;
    };
    struct OrderRested {
        SequenceNumber sequence_number;
        Timestamp timestamp;
        OrderID order_id;
        Symbol symbol;
        Side side;
        Price price;
        Quantity qty;
    };

    struct OrderRejected {
        SequenceNumber sequence_number;
        Timestamp timestamp;
        OrderID order_id;
        ReasonCode reason_code;
    };

    struct OrderReduced {
        SequenceNumber sequence_number;
        Timestamp timestamp;
        OrderID order_id;
        Quantity new_qty;
    };

    struct OrderPartiallyFilled {

        SequenceNumber sequence_number;
        Timestamp timestamp;
        OrderID order_id;
        Quantity filled_qty;
        Quantity remaining_qty;
        Price price;
    };

    struct OrderFilled {
        SequenceNumber sequence_number;
        Timestamp timestamp;
        OrderID order_id;
        Quantity filled_qty;
        Price price;
    };

    struct OrderCanceled {
        SequenceNumber sequence_number;
        Timestamp timestamp;
        OrderID order_id;
        Quantity canceled_qty;
    };

    struct Trade {
        SequenceNumber sequence_number;
        Timestamp timestamp;
        MatchID match_id;
        Symbol symbol;
        OrderID buy_order_id;
        OrderID sell_order_id;
        Price price;
        Quantity qty;
    };

}