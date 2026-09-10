#pragma once

#include "storage.h"

/**
 * @brief 内部接口，用于进行 binding 初始化
 */
static inline void logconduit_detail_initialize_binding(logconduit_logger_binding_t binding) {
  logconduit_detail_per_dso_logger_binding = binding;
}