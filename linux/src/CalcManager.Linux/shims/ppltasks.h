// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// Minimal Linux replacement for MSVC <ppltasks.h> (Parallel Patterns Library).
//
// The engine uses only a tiny synchronous subset of PPL:
//   - concurrency::task<T>            (default-constructible, from a value)
//   - concurrency::task_from_result() (free function)
//   - task<T>::then()                 (immediate continuation, returns task<U>)
//
// Currency web loading in this port is driven from the C# side (Frankfurter),
// so the C++ currency async path only needs to compile and behave
// synchronously. This eager implementation runs continuations inline on the
// calling thread — no thread pool, no scheduler.
//
// This is NOT a general PPL replacement. Do not use it outside CalcManager.
#pragma once

#include <functional>
#include <optional>
#include <utility>

namespace concurrency
{
    template <typename T>
    class task
    {
    public:
        task() = default;

        explicit task(T value)
            : m_value(std::move(value))
        {
        }

        // Continuation: runs immediately, wraps the result in a new task.
        template <typename F>
        auto then(F&& cont) const -> task<decltype(cont(std::declval<T>()))>
        {
            using U = decltype(cont(std::declval<T>()));
            if (!m_value.has_value())
            {
                return task<U>();
            }
            return task<U>(cont(*m_value));
        }

        T get() const
        {
            return *m_value;
        }

        bool valid() const
        {
            return m_value.has_value();
        }

    private:
        std::optional<T> m_value;
    };

    template <typename T>
    task<T> task_from_result(T value)
    {
        return task<T>(std::move(value));
    }

} // namespace concurrency
