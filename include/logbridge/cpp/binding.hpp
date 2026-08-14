#pragma once

#include "../core.h"
#include "../detail/binding_init.h"
#include "adapter.hpp"

namespace logbridge {

/**
 * @brief 初始化当前二进制镜像中的 LogBridge Binding。传入的 Binding 会被复制到当前二进制镜像对应的 LogBridge 存储中。
 *
 * @note
 * LogBridge 不负责 Binding 中 context 指向对象的生命周期管理。
 * context 以及回调函数访问到的所有对象，都必须在当前二进制镜像仍可能产生日志期间保持有效。
 * 当前设计要求每个二进制镜像生命周期内最多初始化一次。
 * 重复初始化属于调用方违反前置条件，LogBridge 不负责检测。
 */
inline void initialize(logbridge_logger_binding_t binding) noexcept {
  logbridge_detail_initialize_binding(binding);
}

/**
 * @brief 使用 C++ Logger 与 Adapter Policy 初始化当前二进制镜像中的 LogBridge Binding。Logger 类型会自动推导，调用方只需要显式指定 Policy：
 *
 * @note
 * LogBridge 不拥有 logger。
 * logger 必须在当前二进制镜像仍可能产生日志期间保持有效。
 * 当前设计要求每个二进制镜像生命周期内最多初始化一次。
 * 重复初始化属于调用方违反前置条件，LogBridge 不负责检测。
 */
template <typename Policy, typename Logger>
inline void initialize(Logger& logger) noexcept {
  logbridge_detail_initialize_binding(
      adapter<Logger, Policy>::make_binding(logger));
}

}  // namespace logbridge