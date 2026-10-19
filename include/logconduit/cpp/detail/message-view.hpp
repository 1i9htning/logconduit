#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <string>

#include "config.hpp"

#if LOGCONDUIT_CPP_STANDARD >= 201703L
#include <string_view>
#endif

namespace logconduit {
namespace detail {

struct message_view {
  const char* data;
  std::uint32_t length;
};

template <typename T>
struct message_traits;

/**
 * @details
 * 适用于消息长度恰好为 N - 1 的字符串字面量和定长字符数组，
 * 不执行运行时长度验证，也不调用 strlen()。
 */
template <std::size_t N>
struct message_traits<char[N]> {
  static message_view make(const char (&message)[N]) noexcept {
    static_assert((N - 1) <= UINT32_MAX, "Logconduit message exceeds uint32_t length limit");
    return {
        message,
        static_cast<std::uint32_t>(N - 1),
    };
  }
};

/**
 * @details
 * 如果 string 的长度超过 UINT32_MAX，Debug 模式下会断言，但是 Release 模式下会出现未定义行为。
 */
template <>
struct message_traits<std::string> {
  static message_view make(const std::string& message) noexcept {
    assert((message.size() <= std::numeric_limits<std::uint32_t>::max()) && "Logconduit message exceeds uint32_t length limit");
    return {
        message.data(),
        static_cast<std::uint32_t>(message.size()),
    };
  }
};

template <typename T>
inline message_view make_message_view(const T& message) noexcept {
  return message_traits<T>::make(message);
}

#if LOGCONDUIT_CPP_STANDARD >= 201703L
template <>
struct message_traits<std::string_view> {
  static message_view make(const std::string_view& message) noexcept {
    assert(message.size() <= std::numeric_limits<std::uint32_t>::max() && "Logconduit message exceeds uint32_t length limit");
    return {
        message.data(),
        static_cast<std::uint32_t>(message.size()),
    };
  }
};
#endif

}  // namespace detail
}  // namespace logconduit