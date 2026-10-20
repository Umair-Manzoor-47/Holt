#pragma once

#include <algorithm>
#include <array>
#include <cstdint>
#include <string_view>
#include <type_traits>

namespace holt::core
{
    using Price = std::int64_t;

    using Quantity = std::uint32_t;

    using OrderID = std::uint64_t;

    using SequenceNumber = std::uint64_t;

    using Timestamp = std::uint64_t;

    using ParticipantID = std::uint32_t;

    using MatchID = std::uint64_t;

    inline constexpr Price PRICE_SCALE = 10000;

    enum class Side : std::uint8_t {
        BUY,
        SELL,
    };

    enum class OrderType : std::uint8_t {
        LIMIT,
        MARKET,
        IOC,
        FOK,
        GTC,
        STOP,
        STOP_LIMIT,
        ICEBERG,
        POST_ONLY
    };
}