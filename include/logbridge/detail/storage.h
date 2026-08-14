#pragma once

#include "../core.h"
#include "platform.h"

/**
 * @brief 动态库唯一 binding 实例
 */
LOGBRIDGE_DETAIL_PER_DSO_STORAGE logbridge_logger_binding_t logbridge_detail_per_dso_logger_binding = {
    NULL,
    NULL,
    NULL,
};

static inline const logbridge_logger_binding_t* logbridge_detail_logger_binding(void) {
  return &logbridge_detail_per_dso_logger_binding;
}
