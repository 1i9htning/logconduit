#ifndef DEMO_CPP_LIBRARY_H
#define DEMO_CPP_LIBRARY_H

// [SEG] logconduit:core
#include <stdint.h>

/**
 * @brief ABI 可靠的布尔值
 */
typedef uint32_t demo_cpp_bool_t;
#define DEMO_CPP_TRUE ((demo_cpp_bool_t)1u)
#define DEMO_CPP_FALSE ((demo_cpp_bool_t)0u)

/**
 * @brief 日志等级
 */
typedef uint32_t demo_cpp_level_t;
#define DEMO_CPP_LEVEL_TRACE ((demo_cpp_level_t)100u)
#define DEMO_CPP_LEVEL_DEBUG ((demo_cpp_level_t)200u)
#define DEMO_CPP_LEVEL_INFO ((demo_cpp_level_t)300u)
#define DEMO_CPP_LEVEL_WARN ((demo_cpp_level_t)400u)
#define DEMO_CPP_LEVEL_ERROR ((demo_cpp_level_t)500u)
#define DEMO_CPP_LEVEL_FATAL ((demo_cpp_level_t)600u)

/**
 * @brief 源码位置
 */
typedef struct demo_cpp_source_location {
  const char* file_name;
  const char* function_name;
  uint32_t line;
} demo_cpp_source_location_t;

/**
 * @brief 日志器绑定
 */
typedef demo_cpp_bool_t (*demo_cpp_enabled_callback_t)(void* context, demo_cpp_level_t level);
typedef void (*demo_cpp_log_callback_t)(void* context, demo_cpp_level_t level, const demo_cpp_source_location_t* source, const char* message, uint32_t length);
typedef struct demo_cpp_logger_binding {
  void* context;
  demo_cpp_enabled_callback_t enabled_callback;
  demo_cpp_log_callback_t log_callback;
} demo_cpp_logger_binding_t;
// [ENDSEG] logconduit:core

// [SEG] logconduit:cpp_adapter
#ifdef __cplusplus

// 请将此生成片段置于全局作用域，且位于所有 extern "C" 块之外。
#include <type_traits>
#include <utility>

namespace demo_cpp {

    template <typename Policy, typename Logger>
    void initialize(Logger& logger) noexcept;

namespace adapter_detail {

class initializer {
private:
  // 导出私有桥接入口，使使用方的模板能够访问当前 DSO 的存储。
  __attribute__((visibility("default")))
  static void initialize_binding(demo_cpp_logger_binding_t binding) noexcept;

  template <typename Policy, typename Logger>
  friend void ::demo_cpp::initialize(Logger& logger) noexcept;
};

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
                           std::declval<demo_cpp_level_t>())))>::type> : std::true_type {};

template <typename Logger, typename Policy, typename = void>
struct has_log : std::false_type {};

template <typename Logger, typename Policy>
struct has_log<Logger, Policy,
               typename make_void<decltype(Policy::log(
                   std::declval<Logger&>(),
                   std::declval<demo_cpp_level_t>(),
                   std::declval<const demo_cpp_source_location_t&>(),
                   std::declval<const char*>(),
                   std::declval<uint32_t>()))>::type> : std::true_type {};

}  // namespace adapter_detail

/**
 * @brief 将 Logger 引用适配为公共回调 ABI，不拥有 Logger 对象。
 * @details
 * Policy 必须提供 static log(Logger&, demo_cpp_level_t,
 * const demo_cpp_source_location_t&, const char*, uint32_t)。
 * static enabled(Logger&, demo_cpp_level_t) 为可选接口；未提供时，
 * 所有日志等级均启用。其返回值必须能够转换为 bool。
 *
 * 在绑定仍可能被使用期间，Logger 及 Policy 使用的所有对象都必须保持存活。
 * 回调函数是 noexcept 的：任何从 Policy 抛出并逸出的异常都会导致进程终止，
 * 而不会跨越 C 回调边界。
 */
template <typename Logger, typename Policy>
struct adapter {
  static demo_cpp_logger_binding_t make_binding(Logger& logger) noexcept {
    static_assert(adapter_detail::has_log<Logger, Policy>::value,
                  "Adapter Policy must provide a compatible static log()");

    demo_cpp_logger_binding_t binding = {
        &logger,
        select_enabled_callback(typename adapter_detail::has_enabled<Logger, Policy>::type()),
        &log_callback,
    };
    return binding;
  }

private:
  static demo_cpp_bool_t enabled_callback(void* context, demo_cpp_level_t level) noexcept {
    Logger& logger = *static_cast<Logger*>(context);
    return Policy::enabled(logger, level) ? DEMO_CPP_TRUE : DEMO_CPP_FALSE;
  }

  static void log_callback(void* context, demo_cpp_level_t level,
                           const demo_cpp_source_location_t* source,
                         const char* message, uint32_t length) noexcept {
    Logger& logger = *static_cast<Logger*>(context);
    Policy::log(logger, level, *source, message, length);
  }

  static demo_cpp_enabled_callback_t select_enabled_callback(std::true_type) noexcept {
    return &enabled_callback;
  }

  static demo_cpp_enabled_callback_t select_enabled_callback(std::false_type) noexcept {
    return nullptr;
  }
};

/**
 * @brief 使用 Logger 初始化当前二进制镜像的私有存储，不拥有 Logger 对象。
 * @details
 * 每个二进制镜像最多初始化一次，且必须在产生日志前完成。Logger 及所有
 * 回调所依赖的对象，都必须在当前二进制镜像
 * 仍可能产生日志期间保持存活。
 */
template <typename Policy, typename Logger>
inline void initialize(Logger& logger) noexcept {
  adapter_detail::initializer::initialize_binding(adapter<Logger, Policy>::make_binding(logger));
}

}  // namespace demo_cpp

#endif  // __cplusplus
// [ENDSEG] logconduit:cpp_adapter

#ifdef __cplusplus
extern "C" {
#endif
__attribute__((visibility("default"))) unsigned demo_cpp_emit(void);
__attribute__((visibility("default"))) unsigned demo_cpp_message_evaluations(void);
#ifdef __cplusplus
}
#endif

#endif
