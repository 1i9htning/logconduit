#pragma once

#include <type_traits>
#include <utility>

#include "../core.h"

namespace logconduit {

namespace adapter_detail {

template <typename...>
struct make_void {
  typedef void type;
};

template <typename Logger, typename Policy, typename = void>
struct has_enabled : std::false_type {};

template <typename Logger, typename Policy>
struct has_enabled<Logger, Policy,
                   typename make_void<decltype(static_cast<bool>(
                       Policy::enabled(
                           std::declval<Logger&>(),
                           std::declval<logconduit_level_t>())))>::type> : std::true_type {};

template <typename Logger, typename Policy, typename = void>
struct has_log : std::false_type {};

template <typename Logger, typename Policy>
struct has_log<Logger, Policy,
               typename make_void<decltype(Policy::log(
                   std::declval<Logger&>(),
                   std::declval<logconduit_level_t>(),
                   std::declval<const logconduit_source_location_t&>(),
                   std::declval<const char*>(),
                   std::declval<uint32_t>()))>::type> : std::true_type {};

}  // namespace adapter_detail

/**
 * @details
 * Policy 必须提供：
 * static void log(
 *     Logger& logger,
 *     logconduit_level_t level,
 *     const logconduit_source_location_t& source,
 *     const char* message,
 *     uint32_t length);
 *
 * Policy 可选提供
 * static bool enabled(
 *     Logger& logger,
 *     logconduit_level_t level);
 *
 * Logconduit 不拥有 Logger 对象，在使用返回的绑定对象期间，Logger 必须保持存活。
 * 适配器回调函数是 noexcept 的。任何从 Policy 抛出并逸出的异常都会导致进程终止，而不会跨越 C 回调边界。
 */
template <typename Logger, typename Policy>
struct adapter {
  static logconduit_logger_binding_t make_binding(Logger& logger) noexcept {
    static_assert(adapter_detail::has_log<Logger, Policy>::value, "Logconduit adapter Policy must provide a compatible static log()");

    logconduit_logger_binding_t binding = {
        &logger,
        select_enabled_callback(typename adapter_detail::has_enabled<Logger, Policy>::type()),
        &log_callback,
    };

    return binding;
  }

private:
  static logconduit_bool_t enabled_callback(void* context, logconduit_level_t level) noexcept {
    Logger& logger = *static_cast<Logger*>(context);
    return Policy::enabled(logger, level) ? LOGCONDUIT_TRUE : LOGCONDUIT_FALSE;
  }

  static void log_callback(void* context, logconduit_level_t level, const logconduit_source_location_t* source, const char* message, uint32_t length) noexcept {
    Logger& logger = *static_cast<Logger*>(context);

    Policy::log(logger, level, *source, message, length);
  }

  static logconduit_enabled_callback_t select_enabled_callback(std::true_type) noexcept {
    return &enabled_callback;
  }

  static logconduit_enabled_callback_t select_enabled_callback(std::false_type) noexcept {
    return nullptr;
  }
};

}  // namespace logconduit