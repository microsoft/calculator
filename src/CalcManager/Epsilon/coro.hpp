// SPDX-License-Identifier: MIT
// Copyright (c) 2026-present Tian Liao

#ifndef EPSILON_INC_CORO_HPP
#define EPSILON_INC_CORO_HPP

// std
#include <coroutine>
#include <exception>
#include <stdexcept>
#include <utility>
#include <variant>

// epsilon
#include "tmp.hpp"

namespace epx::coro {

template <class T>
class lazy_promise;

template <class T>
class lazy {
  friend class lazy_promise<T>;

 public:
  lazy(const lazy&) = delete;
  lazy(lazy&& rhs) noexcept : coro_(std::exchange(rhs.coro_, {})) {}
  ~lazy() {
    if (static_cast<bool>(coro_)) coro_.destroy();
  }

  class awaiter;
  awaiter operator co_await() noexcept;

  T get();

 private:
  explicit lazy(std::coroutine_handle<lazy_promise<T>> coro) : coro_(coro) {}
  std::coroutine_handle<lazy_promise<T>> coro_;
};

template <class T>
class lazy_promise {
  friend class lazy<T>::awaiter;

 public:
  lazy<T> get_return_object() noexcept { return lazy<T>{std::coroutine_handle<lazy_promise>::from_promise(*this)}; }
  constexpr auto initial_suspend() noexcept { return std::suspend_always{}; }
  constexpr auto final_suspend() noexcept {
    struct final_awaiter : std::suspend_always {
      std::coroutine_handle<> await_suspend(std::coroutine_handle<lazy_promise> handle) noexcept {
        auto cont = handle.promise().cont_;
        if (cont) {
          return cont;
        } else {
          return std::noop_coroutine();
        }
      }
    };
    return final_awaiter{};
  }
  template <class U>
  void return_value(U&& value) {
    assert(std::holds_alternative<std::monostate>(state_));
    state_ = std::forward<U>(value);
  }
  void unhandled_exception() noexcept { state_ = std::current_exception(); }

 private:
  std::coroutine_handle<> cont_;
  std::variant<std::monostate, T, std::exception_ptr> state_ = std::monostate{};
};

template <class T>
class lazy<T>::awaiter : public std::suspend_always {
 public:
  explicit awaiter(std::coroutine_handle<lazy_promise<T>> handle) : coro_(handle) {}
  auto await_suspend(std::coroutine_handle<> cont) noexcept {
    coro_.promise().cont_ = cont;
    return coro_;
  }
  T await_resume() {
    return std::visit(
        tmp::overloads{
            [](T& val) -> T { return std::move(val); },                                 // normal path
            [](std::monostate) -> T { std::terminate(); },                              // unassigned
            [](std::exception_ptr& ex) -> T { std::rethrow_exception(std::move(ex)); }  // contains exception
        },
        coro_.promise().state_);
  }

  T get() {
    coro_();
    return await_resume();
  }

 private:
  std::coroutine_handle<lazy_promise<T>> coro_;
};
template <class T>
typename lazy<T>::awaiter lazy<T>::operator co_await() noexcept {
  return lazy<T>::awaiter{coro_};
}

template <class T>
T lazy<T>::get() {
  return lazy<T>::awaiter{coro_}.get();
}

struct forget {};

}  // namespace epx::coro

namespace std {

template <class T, class... Args>
struct coroutine_traits<epx::coro::lazy<T>, Args...> {
  using promise_type = epx::coro::lazy_promise<T>;
};

template <class... Args>
struct coroutine_traits<epx::coro::forget, Args...> {
  struct promise_type {
    constexpr auto get_return_object() noexcept { return epx::coro::forget{}; }
    constexpr auto initial_suspend() noexcept { return std::suspend_never{}; }
    constexpr auto final_suspend() noexcept { return std::suspend_never{}; }
    constexpr void return_void() noexcept {}
    void unhandled_exception() { std::terminate(); }
  };
};

}  // namespace std

#endif  // EPSILON_INC_CORO_HPP
