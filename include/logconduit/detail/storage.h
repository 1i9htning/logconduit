#pragma once

#include "../core.h"
#include "platform.h"

/**
 * @brief 动态库唯一 binding 实例
 */
LOGCONDUIT_DETAIL_PER_DSO_STORAGE logconduit_logger_binding_t logconduit_detail_per_dso_logger_binding = {
    NULL,
    NULL,
    NULL,
};

static inline const logconduit_logger_binding_t* logconduit_detail_logger_binding(void) {
  return &logconduit_detail_per_dso_logger_binding;
}
