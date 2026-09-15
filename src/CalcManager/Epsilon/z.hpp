// SPDX-License-Identifier: MIT
// Copyright (c) 2026-present Tian Liao

#ifndef EPSILON_INC_Z_HPP
#define EPSILON_INC_Z_HPP

// std
#include <algorithm>
#include <bit>
#include <cassert>
#include <climits>
#include <concepts>
#include <limits>
#include <ranges>

// epx
#include "t.hpp"

namespace epx {

namespace details {

template <container C>
constexpr z<C> zero() {
  return z<C>{};
}

template <container C>
constexpr z<C> one() {
  return z<C>{.digits = {1}};
}

template <container C>
constexpr z<C> base() {
  return z<C>{.digits = {4}};
}

template <container C>
constexpr z<C> ten() {
  return z<C>{.digits = {10}};
}

template <container C>
constexpr z<C> pow10(unsigned exp) {
  auto res = one<C>();
  for (unsigned i = 0; i < exp; ++i) {
    res = mul_n(res, ten<C>());
  }
  return res;
}

template <class MaxD, std::unsigned_integral T>
constexpr auto umul(T lhs, T rhs) {
  struct result_t {
    T p0;
    T p1;
  };

  if constexpr (sizeof(T) >= sizeof(MaxD)) {
    constexpr T s = sizeof(T) * 4u;
    constexpr T mask = std::numeric_limits<T>::max() >> s;
    T a0 = lhs & mask, b0 = rhs & mask;
    T a1 = lhs >> s, b1 = rhs >> s;
    T ll = a0 * b0, lh = a0 * b1, hl = a1 * b0, hh = a1 * b1;
    T w = lh + (ll >> s);
    w += hl;
    if (w < hl) hh += mask + 1u;
    return result_t{.p0 = (w << s) + (ll & mask), .p1 = hh + (w >> s)};
  } else {
    constexpr unsigned s = sizeof(T) * 8u;
    MaxD l = lhs, r = rhs;
    auto prod = l * r;
    return result_t{.p0 = static_cast<T>(prod), .p1 = static_cast<T>(prod >> s)};
  }
}

// u0 - LSD, u1 - MSD
template <class D>
constexpr auto div_2d(D u0, D u1, D v) {
  using W = wide_digit_type<D>;
  struct result_t {
    W q;
    D r;
  };
  assert(v != 0);
  W u = (static_cast<W>(u1) << (sizeof(D) * CHAR_BIT)) | u0;
  return result_t{.q = static_cast<W>(u / v), .r = static_cast<D>(u % v)};
}

template <container C>
constexpr auto bit_shift(C& digits, int offset) {
  using D = typename C::value_type;
  assert((sizeof(D) * CHAR_BIT) > (size_t)std::abs(offset));
  if (offset > 0) {
    // left shift
    const D mask = ((1u << offset) - 1) << (sizeof(D) * CHAR_BIT - offset);
    D cy = 0;
    for (auto& d : digits) {
      D t = (d << offset) | cy;
      cy = (d & mask) >> (sizeof(D) * CHAR_BIT - offset);
      d = t;
    }
    return cy;  // return the left-shifted out bits
  } else if (offset < 0) {
    offset = std::abs(offset);
    // right shift
    const D mask = (1u << offset) - 1;
    D cy = 0;
    for (auto d = digits.rbegin(); d != digits.rend(); ++d) {
      D t = (*d >> offset) | cy;
      cy = (*d & mask) << (sizeof(D) * CHAR_BIT - offset);
      *d = t;
    }
    return D{0};  // drop the right-shifted bits
  } else {
    return D{0};
  }
}

template <container C>
constexpr int bit_length(const C& digits) noexcept {
  using D = typename C::value_type;
  size_t dcount = std::ranges::size(digits);
  if (dcount == 0) return 0;
  D msd = digits[dcount - 1];
  return static_cast<int>(dcount * sizeof(D) * CHAR_BIT) - std::countl_zero(msd);
};
}  // namespace details

template <container C, std::integral T>
constexpr z<C> create(T val) {
  z<C> res;
  if (val == 0) {
    return res;
  }
  if constexpr (std::is_signed_v<T>) {
    res.sgn = val < 0 ? sign::negative : sign::positive;
    val = val < 0 ? -val : val;
  }
  using D = typename z<C>::digit_type;
  if constexpr (sizeof(T) <= sizeof(D)) {
    res.digits.push_back(static_cast<D>(val));
  } else {
    while (val > 0) {
      res.digits.push_back(static_cast<D>(val));
      val >>= (sizeof(D) * CHAR_BIT);
    }
  }
  return res;
}

template <container C>
constexpr bool is_zero(const z<C>& num) noexcept {
  return std::ranges::size(num.digits) == 0;
}

template <container C>
constexpr bool is_negative(const z<C>& num) noexcept {
  return num.sgn == sign::negative;
}

template <container C>
constexpr z<C>& normalize(z<C>& num) noexcept {
  using D = typename z<C>::digit_type;
  auto pos = std::find_if(num.digits.rbegin(), num.digits.rend(), [](D x) { return x != 0; });
  num.digits.resize(std::ranges::size(num.digits) - std::ranges::distance(num.digits.rbegin(), pos));
  num.sgn = num.digits.empty() == false ? num.sgn : sign::positive;
  return num;
}

template <container C>
constexpr z<C>& negate(z<C>& num) noexcept {
  if (is_zero(num)) {
    return num;
  }
  num.sgn = is_negative(num) ? sign::positive : sign::negative;
  return num;
}

template <container C>
constexpr int cmp_n(const z<C>& lhs, const z<C>& rhs) {
  if (std::ranges::size(lhs.digits) != std::ranges::size(rhs.digits)) {
    return std::ranges::size(lhs.digits) < std::ranges::size(rhs.digits) ? -1 : 1;
  }

  auto res = std::lexicographical_compare_three_way(std::ranges::crbegin(lhs.digits), std::ranges::crend(lhs.digits),
                                                    std::ranges::crbegin(rhs.digits), std::ranges::crend(rhs.digits));
  if (res < 0) return -1;
  if (res > 0) return 1;
  return 0;
}

template <container C>
constexpr z<C> add_n(const z<C>& lhs, const z<C>& rhs) {
  using D = typename z<C>::digit_type;
  z<C> r;
  r.digits.reserve(std::max(std::ranges::size(lhs.digits), std::ranges::size(rhs.digits)) + 1);

  const auto& [a, b] = std::ranges::size(lhs.digits) <= std::ranges::size(rhs.digits)
                           ? std::tie(lhs.digits, rhs.digits)
                           : std::tie(rhs.digits, lhs.digits);
  size_t i = 0;
  D cy = 0;
  for (; i < std::ranges::size(a); ++i) {
    D s1 = a[i] + b[i];
    D cy1 = s1 < a[i];
    D s2 = s1 + cy;
    D cy2 = s2 < s1;
    cy = cy1 | cy2;
    r.digits.push_back(s2);
  }
  for (; i < std::ranges::size(b); ++i) {
    D s = b[i] + cy;
    cy = s < b[i];
    r.digits.push_back(s);
  }
  if (cy > 0) {
    r.digits.push_back(1u);
  }
  return r;
}

template <container C>
constexpr z<C> sub_n(const z<C>& lhs, const z<C>& rhs) {
  assert(std::ranges::size(lhs.digits) >= std::ranges::size(rhs.digits));
  using D = typename z<C>::digit_type;
  z<C> r;
  const auto& a = lhs.digits;
  const auto& b = rhs.digits;

  r.digits.reserve(a.size());
  D borrow = 0;
  size_t i = 0;

  for (; i < b.size(); ++i) {
    D d1 = a[i];
    D d2 = b[i];
    D diff = d1 - d2 - borrow;
    borrow = borrow ? (d1 <= d2) : (d1 < d2);
    r.digits.push_back(diff);
  }
  for (; i < a.size(); ++i) {
    D d1 = a[i];
    D diff = d1 - borrow;
    borrow = (d1 < borrow);
    r.digits.push_back(diff);
  }
  assert(borrow == 0);
  normalize(r);
  return r;
}

template <container C>
constexpr z<C> mul_n(const z<C>& lhs, const z<C>& rhs) {
  using D = typename z<C>::digit_type;
  if (is_zero(lhs) || is_zero(rhs)) {
    return details::zero<C>();
  }

  z<C> r;
  r.digits.resize(std::ranges::size(lhs.digits) + std::ranges::size(rhs.digits));

  const auto& a = lhs.digits;
  const auto& b = rhs.digits;
  for (size_t j = 0; j < std::ranges::size(b); ++j) {
    D cy = 0;
    for (size_t i = 0; i < std::ranges::size(a); ++i) {
      auto [p0, p1] = details::umul<default_digit_type>(a[i], b[j]);
      p0 += cy;
      cy = (p0 < cy ? 1u : 0u) + p1;
      r.digits[i + j] += p0;
      if (r.digits[i + j] < p0) ++cy;
    }
    r.digits[j + std::ranges::size(a)] = cy;
  }
  normalize(r);
  return r;
}

template <container C>
constexpr auto div_n(const z<C>& u, typename z<C>::digit_type v) {
  using D = typename z<C>::digit_type;
  static_assert(sizeof(D) <= sizeof(max_digit_type));

  struct {
    z<C> q;
    D r;
  } res{};

  auto& w = res.q.digits;
  auto n = std::ranges::size(u.digits);
  w.resize(n);
  for (auto i = 0uz; i < n; ++i) {
    auto j = n - 1 - i;
    auto [q, r] = details::div_2d(u.digits[j], res.r, v);
    w[j] = static_cast<D>(q);
    res.r = r;
  }
  normalize(res.q);
  return res;
}

template <container C>
constexpr auto div_n(z<C> lhs, z<C> rhs) {
  struct result_t {
    z<C> q;
    z<C> r;
  };

  if (is_zero(rhs)) [[unlikely]] {
    throw divide_by_zero_error{};
  }

  auto rel = cmp_n(lhs, rhs);
  if (rel > 0) {
    if (std::ranges::size(rhs.digits) > 1) {
      using D = typename z<C>::digit_type;
      using W = wide_digit_type<D>;
      constexpr W b = W{1} << (sizeof(D) * CHAR_BIT);
      constexpr D mask = static_cast<D>(b - 1);

      auto& u = lhs.digits;
      auto& v = rhs.digits;
      auto n = std::ranges::size(v);
      auto m = std::ranges::size(u) - n;

      z<C> q;
      q.digits.resize(m + 1);

      // D1. [Normalize]
      const auto s = std::countl_zero(v[n - 1]);
      details::bit_shift(v, (int)s);
      u.push_back(details::bit_shift(u, (int)s));  // this ensures u[m+n] exists.

      // D2. [Initialize j]
      for (auto l = 0uz; l <= m; ++l) {
        auto j = m - l;

        // D3. [Calculate qhat]
        auto [qhat, rhat] = details::div_2d(u[j + n - 1], u[j + n], v[n - 1]);
        while (qhat >= b || qhat * v[n - 2] > rhat * b + u[j + n - 2]) {
          --qhat;
          rhat += v[n - 1];
          if (rhat < v[n - 1]) break;  // continue if rhat < b.
        }

        // D4. [Multiply and subtract]
        using I = std::make_signed_t<W>;
        I borrow = 0, diff;
        for (auto i = 0uz; i < n; ++i) {  // u[j+n]u[j+n-1]...u[j], v[n-1]v[n-2]...v[0]
          W p = qhat * v[i];
          diff = W{u[i + j]} - borrow - static_cast<W>(p & mask);
          u[i + j] = static_cast<D>(diff);
          borrow = (p >> (sizeof(D) * CHAR_BIT)) - (diff >> (sizeof(D) * CHAR_BIT));
        }
        diff = I{u[j + n]} - borrow;
        u[j + n] = static_cast<D>(diff);
        q.digits[j] = static_cast<D>(qhat);

        // D5. [Test remainder]
        if (diff < 0) {
          // D6. [Add back]
          --q.digits[j];
          I carry = 0;
          for (auto i = 0uz; i < n; ++i) {
            W sum = W{u[i + j]} + W{v[i]} + carry;
            u[i + j] = static_cast<D>(sum);
            carry = sum >> (sizeof(D) * CHAR_BIT);
          }
          u[j + n] = u[j + n] + static_cast<D>(carry);
        }
      }  // D7. [Loop on j]
      // D8. [Unnormalize]
      details::bit_shift(u, -s);
      normalize(lhs);
      return result_t{.q = std::move(normalize(q)), .r = std::move(lhs)};
    } else {
      assert(std::ranges::size(rhs.digits) == 1);
      auto res = div_n(lhs, rhs.digits[0]);
      z<C> r{.digits = {res.r}};
      normalize(r);
      return result_t{.q = std::move(res.q), .r = std::move(r)};
    }
  } else if (rel < 0) {
    return result_t{.q = details::zero<C>(), .r = std::move(lhs)};
  } else {
    return result_t{.q = details::one<C>(), .r = details::zero<C>()};
  }
}

template <container C>
constexpr z<C> add(const z<C>& lhs, const z<C>& rhs) {
  z<C> r;
  if (lhs.sgn == rhs.sgn) {
    r = add_n(lhs, rhs);
    r.sgn = lhs.sgn;
  } else {
    const auto* minuend = &lhs;
    const auto* substrahend = &rhs;
    if (cmp_n(lhs, rhs) < 0) {
      minuend = &rhs;
      substrahend = &lhs;
    }
    r = sub_n(*minuend, *substrahend);
    r.sgn = std::ranges::size(r.digits) > 0 ? minuend->sgn : sign::positive;
  }
  return r;
}

template <container C>
constexpr z<C> sub(const z<C>& lhs, z<C> rhs) {
  return add(lhs, negate(rhs));
}

template <container C>
constexpr z<C> mul(const z<C>& lhs, const z<C>& rhs) {
  z<C> r = mul_n(lhs, rhs);
  if (is_zero(r)) {
    return r;
  }
  r.sgn = lhs.sgn == rhs.sgn ? sign::positive : sign::negative;
  return r;
}

template <container C>
constexpr auto div(z<C> lhs, z<C> rhs) {
  struct result_t {
    z<C> q;
    z<C> r;
  };
  auto sgn = lhs.sgn == rhs.sgn ? sign::positive : sign::negative;
  auto [q, r] = div_n(std::move(lhs), std::move(rhs));
  result_t res = {.q = std::move(q), .r = std::move(r)};

  res.r.sgn = lhs.sgn;
  res.q.sgn = sgn;
  return res;
}

template <container C>
constexpr auto floor_div(z<C> lhs, z<C> rhs) {
  struct result_t {
    z<C> q;
    z<C> r;
  };

  auto sgn = lhs.sgn == rhs.sgn ? sign::positive : sign::negative;
  auto [q, r] = div_n(std::move(lhs), rhs);
  result_t res = {.q = std::move(q), .r = std::move(r)};

  if (sgn == sign::negative && !is_zero(res.r)) {
    res.q = add_n(res.q, details::one<C>());
    res.r = sub_n(rhs, res.r);
  }

  res.q.sgn = sgn;
  res.r.sgn = rhs.sgn;
  return res;
}

template <container C>
constexpr auto ceil_div(z<C> lhs, z<C> rhs) {
  struct result_t {
    z<C> q;
    z<C> r;
  };

  auto sgn = lhs.sgn == rhs.sgn ? sign::positive : sign::negative;
  auto [q, r] = div_n(std::move(lhs), rhs);
  result_t res = {.q = std::move(q), .r = std::move(r)};

  if (sgn == sign::positive && !is_zero(res.r)) {
    res.q = add_n(res.q, details::one<C>());
    res.r = sub_n(rhs, res.r);
  }
  res.q.sgn = sgn;
  res.r.sgn = rhs.sgn == sign::positive ? sign::negative : sign::positive;
  return res;
}

template <container C>
constexpr z<C>& mul_2exp(z<C>& val, int exp) {
  using D = typename z<C>::digit_type;
  constexpr int dbits = static_cast<int>(sizeof(D) * CHAR_BIT);
  if (is_zero(val) || exp == 0) {
    return val;
  }
  if (std::abs(exp) < dbits) {
    D cy = details::bit_shift(val.digits, exp);
    if (cy > 0) {
      val.digits.push_back(cy);
    }
    normalize(val);
  } else if (exp > 0) {
    size_t digit_shift = exp / dbits;
    int bit_shift = exp % dbits;
    if (bit_shift > 0) {
      D cy = details::bit_shift(val.digits, bit_shift);
      if (cy > 0) {
        val.digits.push_back(cy);
      }
    }
    val.digits.insert(val.digits.begin(), digit_shift, 0u);
  } else if (exp < 0) {
    size_t digit_shift = (-exp) / dbits;
    int bit_shift = (-exp) % dbits;
    if (digit_shift >= std::ranges::size(val.digits)) {
      val.digits.clear();
      val.sgn = sign::positive;
    } else {
      val.digits.erase(val.digits.begin(), val.digits.begin() + digit_shift);
      if (bit_shift > 0) {
        details::bit_shift(val.digits, -bit_shift);
        normalize(val);
      }
    }
  }
  return val;
}

template <container C>
constexpr z<C>& mul_4exp(z<C>& val, int exp) {
  return mul_2exp(val, 2 * exp);
}

template <container C>
constexpr z<C> mul_2exp(const z<C>& val, int exp) {
  auto num = val;
  return mul_2exp(num, exp);
}

template <container C>
constexpr z<C> mul_4exp(const z<C>& val, int exp) {
  auto num = val;
  return mul_4exp(num, exp);
}

template <container C>
constexpr z<C> pow(const z<C>& num, int exp) {
  assert(exp >= 0);
  z<C> res = details::one<C>();
  if (exp > 0) {
    for (int i = 0; i < exp; ++i) {
      res = mul_n(res, num);
    }
  } else if (exp == 0) {
    return res;
  } else [[unlikely]] {
    throw negative_zpow_error{};
  }
  if (is_negative(num) && (exp % 2 == 1)) {
    res.sgn = sign::negative;
  }
  return res;
}

template <container C>
constexpr z<C> root(const z<C>& num, int k) {
  if (is_negative(num)) [[unlikely]] {
    throw negative_radicand_error{};
  }

  if (k == 0) {
    return details::one<C>();
  } else if (k == 1 || is_zero(num)) {
    return num;
  }

  auto x0 = details::one<C>();
  mul_2exp(x0, (details::bit_length(num.digits) + k - 1) / k);

  z<C> x1;
  for (;;) {
    auto t1 = mul_n(create<C>(k - 1), x0);
    auto [t2, _] = div(num, pow(x0, k - 1));
    x1 = div_n(add_n(t1, t2), create<C>(k)).q;
    if (cmp_n(x1, x0) >= 0) {
      return x0;
    }
    x0 = x1;
  }
}

}  // namespace epx

#endif
