// SPDX-License-Identifier: MIT
// Copyright (c) 2026-present Tian Liao

#ifndef EPSILON_INC_OPS_HPP
#define EPSILON_INC_OPS_HPP

#include "z.hpp"

template <epx::container C>
constexpr epx::z<C> operator+(const epx::z<C>& lhs, const epx::z<C>& rhs) {
  return epx::add(lhs, rhs);
}

template <epx::container C>
constexpr epx::z<C> operator-(const epx::z<C>& lhs, epx::z<C> rhs) {
  return epx::add(lhs, epx::negate(rhs));
}

template <epx::container C>
constexpr epx::z<C> operator*(const epx::z<C>& lhs, const epx::z<C>& rhs) {
  return epx::mul(lhs, rhs);
}

template <epx::container C>
constexpr epx::z<C> operator/(epx::z<C> lhs, epx::z<C> rhs) {
  auto [q, _] = epx::div(std::move(lhs), std::move(rhs));
  return q;
}

template <epx::container C>
constexpr epx::z<C> operator%(epx::z<C> lhs, epx::z<C> rhs) {
  auto [_, r] = epx::div(std::move(lhs), std::move(rhs));
  return r;
}

#endif  // EPSILON_INC_OPS_HPP
