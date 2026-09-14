// SPDX-License-Identifier: MIT
// Copyright (c) 2026-present Tian Liao

#ifndef EPSILON_INC_TMP_HPP
#define EPSILON_INC_TMP_HPP

#if defined(_MSC_VER) && !defined(__clang__)
#define _EPSILON_DECL_EMPTY_BASES __declspec(empty_bases)
#else
#define _EPSILON_DECL_EMPTY_BASES
#endif  // _MSC_VER && !__clang__

namespace epx::tmp {

template <class... Fs>
struct _EPSILON_DECL_EMPTY_BASES overloads : Fs... {
  using Fs::operator()...;
};

}  // namespace epx::tmp

#undef _EPSILON_DECL_EMPTY_BASES
#endif  // EPSILON_INC_TMP_HPP
