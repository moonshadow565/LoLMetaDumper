#pragma once
#include <string>
#include <string_view>
#include <algorithm>
#include "mem.hpp"

namespace Version {
    enum class Patch : unsigned int {
        V0 = 0,
        V5_2 = ((5 << 24) | (2 << 16)),
        V5_8 = ((5 << 24) | (8 << 16)),
        V5_21 = ((5 << 24) | (21 << 16)),
        V6_20 = ((6 << 24) | (20 << 16)),
        V7_15 = ((7 << 24) | (16 << 16)),
        V10_11 = ((10 << 24) | (11 << 16)),
        V11_6 = ((11 << 24) | (6 << 16)),
        UNKNOWN = 0xFFFFFFFF
    };

    inline std::string Dump(std::vector<uint8_t> const& data) noexcept {
        auto databeg = reinterpret_cast<char16_t const*>(data.data());
        auto dataend = databeg + (data.size() / 2);
        for (;;) {
            auto const pat = std::u16string_view{ u"\u0001ProductVersion" };
            auto const vtag = std::search(databeg, dataend,
                std::boyer_moore_searcher(pat.cbegin(), pat.cend()));
            if (vtag == dataend) {
                return {};
            }
            auto const vbeg = vtag + pat.size() + 1;
            auto const vend = std::find(vbeg, dataend, u'\u0000');
            auto version = std::string(vend - vbeg, '\0');
            std::transform(vbeg, vend, version.begin(), [](char16_t c) { return static_cast<char>(c); });

            constexpr auto isBadChar = [](char c) { return c == '.' || (c >= '0' && c <= '9'); };
            if (std::find_if_not(version.cbegin(), version.cend(), isBadChar) != version.cend()) {
                databeg = vend;
                continue;
            };

            return version;
        }
        return {};
    }
    
    inline Patch Parse(std::string_view str) noexcept {
        auto const high_beg = str.cbegin();
        auto const high_end = std::find(high_beg, str.cend(), '.');
        if (high_end == str.cend()) {
            return Patch::UNKNOWN;
        }
        auto const low_beg = high_end + 1;
        auto const low_end = std::find(low_beg, str.cend(), '.');
        if (low_end == str.cend()) {
            return Patch::UNKNOWN;
        }
        auto const high = std::stoi(std::string(high_beg, high_end));
        auto const low = std::stoi(std::string(low_beg, low_end));
        return static_cast<Patch>((high << 24) | (low << 16));
    }
}

using Patch = Version::Patch;
