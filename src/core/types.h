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

    enum class OrderStatus : std::uint8_t {
        NEW,
        ACCEPTED,
        PARTIALLY_FILLED,
        FILLED,
        CANCELLED,
    };

    using Symbol = std::array<char, 8>;

    constexpr Symbol make_symbol(std::string_view text) noexcept {
        Symbol symbol {};
        const auto length = std::min(symbol.size(), text.size());

        for (std::size_t i = 0; i < length; ++i) {
            symbol[i] = text[i];
        }

        return symbol;
    }

    constexpr std::string_view symbol_view(const Symbol& symbol) noexcept {
        std::size_t length = 0;
        while (length < symbol.size() && symbol[length] != '\0') {
            ++length;
        }

        return std::string_view(symbol.data(), length);
    }

    struct SymbolLess {
        constexpr bool operator()(const Symbol& lhs,
            const Symbol& rhs) const noexcept {
            return std::lexicographical_compare(lhs.begin(), lhs.end(),
                rhs.begin(), rhs.end());
        }
    };

    static_assert(std::is_trivially_copyable_v<Price>);

}