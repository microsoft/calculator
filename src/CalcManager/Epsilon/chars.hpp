// SPDX-License-Identifier: MIT
// Copyright (c) 2026-present Tian Liao

#ifndef EPSILON_INC_CHARS_HPP
#define EPSILON_INC_CHARS_HPP

// std
#include <cctype>
#include <optional>
#include <string_view>
#include <utility>

// epx
#include "ops.hpp"
#include "r.hpp"
#include "z.hpp"

namespace epx {

template <container C, int B = 10>
constexpr std::optional<z<C>> try_from_chars(std::string_view chars) {
  using D = typename z<C>::digit_type;

  if constexpr (B == 10) {
    const z<C> ten = {.digits = {10}};

    sign sgn;
    if (chars.length() > 1) {
      if (chars.front() == '+' && isdigit(static_cast<unsigned int>(chars[1]))) {
        sgn = sign::positive;
        chars = chars.substr(1);
      } else if (chars.front() == '-' && isdigit(static_cast<unsigned int>(chars[1]))) {
        sgn = sign::negative;
        chars = chars.substr(1);
      } else if (isdigit(static_cast<unsigned int>(chars.front()))) {
        sgn = sign::positive;
      } else {
        return std::nullopt;
      }
    } else {
      sgn = sign::positive;
    }

    z<C> res;
    for (auto ch : chars) {
      if (!isdigit(static_cast<unsigned int>(ch))) {
        return std::nullopt;
      }
      int digit = ch - '0';
      z<C> tmp;
      if (digit > 0) {
        tmp = {.digits = {static_cast<D>(digit)}};
      }
      res = res * ten + tmp;
    }
    if (!is_zero(res)) {
      res.sgn = sgn;
    }
    return res;
  } else {
    static_assert("not implemented.");
  }
}

template <container C, int B = 10>
constexpr std::string to_string(z<C> num) {
  if constexpr (B == 10) {
    if (is_zero(num)) {
      return "0";
    }

    std::string res;
    sign sgn = num.sgn;
    const z<C> ten = {.digits = {10}};
    while (!is_zero(num)) {
      auto [q, r] = div_n(num, ten);
      if (is_zero(r)) {
        res.push_back('0');
      } else {
        res.push_back(static_cast<char>(r.digits[0]) + '0');
      }
      num = std::exchange(q, {});
    }
    if (sgn == sign::negative) {
      res.push_back('-');
    }
    std::ranges::reverse(res);
    return res;
  } else {
    static_assert("not implemented.");
  }
}

template <container C, int B = 10>
constexpr std::string to_string(r<C> num, unsigned int k) {
  constexpr double log_4_10 = 1.66096405;
  constexpr int extra_precision = 20;
  if constexpr (B == 10) {
    auto n = static_cast<int>(log_4_10 * k) + extra_precision;
    auto xn = num.approx(n).get();
    auto sgn = xn.sgn;  // use the absolute value of xn to round towards zero.
    xn.sgn = sign::positive;

    // Round xn / 4^n to k decimal places: floor(xn * 10^k / 4^n + 0.5).
    // Uses the mutating mul_4exp(z<C>&, int) rather than mul_4exp(const z<C>&, int)
    // to avoid copying a 1-element z<C>, which triggers a false-positive -Warray-bounds
    // in GCC 13 on std::vector's __builtin_memmove.
    auto half = details::one<C>();
    mul_4exp(half, n);
    auto d = mul_2exp(xn, 1) * details::pow10<C>(k) + std::move(half);
    mul_4exp(d, -n);  // divide by 4^n
    mul_2exp(d, -1);  // divide by 2
    d.sgn = sgn;      // restore the sign

    auto s = to_string(d);
    size_t len = is_negative(d) ? s.length() - 1 : s.length();
    if (len <= k) {
      s.insert(is_negative(d) ? 1 : 0, k - len + 1, '0');
    }
    if (k > 0) {
      s.insert(s.length() - k, 1, '.');
    }

    return s;
  } else {
    static_assert("not implemented.");
  }
}

}  // namespace epx

#endif  // EPSILON_INC_CHARS_HPP
