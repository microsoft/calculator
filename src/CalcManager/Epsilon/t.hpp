#ifndef EPSILON_INC_T_HPP
#define EPSILON_INC_T_HPP

// std
#include <concepts>
#include <cstdint>
#include <ranges>
#include <stdexcept>
#include <type_traits>
#include <vector>

namespace epx {

using default_digit_type = uint32_t;
using max_digit_type = uint32_t;
using default_container_type = std::vector<default_digit_type>;

struct bad_digit_type;
template <class D>
using wide_digit_type =
    std::conditional_t<sizeof(D) == sizeof(uint8_t), uint16_t,
                       std::conditional_t<sizeof(D) == sizeof(uint16_t), uint32_t,
                                          std::conditional_t<sizeof(D) == sizeof(uint32_t), uint64_t, bad_digit_type>>>;

template <class T>
concept container = std::ranges::random_access_range<T> &&  //
                    std::ranges::sized_range<T> &&          //
                    requires {
                      typename T::value_type;
                      std::is_unsigned_v<typename T::value_type>;
                    } && requires(T c) {
                      c.push_back(typename T::value_type{});
                      c.reserve(size_t{});
                      sizeof(typename T::value_type) < sizeof(max_digit_type);
                    };

enum class sign : uint8_t { positive, negative };

template <container C = default_container_type>
struct z {
  using container_type = C;
  using digit_type = typename C::value_type;
  std::strong_ordering operator<=>(const z&) const = default;

  C digits;  // // least significant digits (LSD)
  sign sgn = sign::positive;
};

struct global_config_tag {};  // allow user-defined configs

template <typename>
constexpr int max_msd = 10000;  // can be overridden by global_config_tag

struct divide_by_zero_error : public std::runtime_error {
  divide_by_zero_error() : std::runtime_error("epx: divide by zero") {}
};

struct msd_overflow_error : public std::runtime_error {
  msd_overflow_error() : std::runtime_error("epx: msd overflow error") {}
};

struct precision_overflow_error : public std::runtime_error {
  precision_overflow_error() : std::runtime_error("epx: precision overflow error") {}
};

struct negative_radicand_error : public std::runtime_error {
  negative_radicand_error() : std::runtime_error("epx: negative radicand error") {}
};

struct negative_zpow_error : public std::runtime_error {
  negative_zpow_error() : std::runtime_error("epx: negative power for z error") {}
};

struct kthroot_too_small_error : public std::runtime_error {
  kthroot_too_small_error() : std::runtime_error("epx: kth-root too small error") {}
};

struct non_positive_log_error : public std::runtime_error {
  non_positive_log_error() : std::runtime_error("epx: logarithm of non-positive number") {}
};

}  // namespace epx

#endif
