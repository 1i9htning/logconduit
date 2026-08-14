#pragma once

#include "storage.h"

/**
 * @brief 内部接口，用于进行 binding 初始化
 */
static inline void logbridge_detail_initialize_binding(logbridge_logger_binding_t binding) {
  logbridge_detail_per_dso_logger_binding = binding;
}