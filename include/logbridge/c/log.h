/**
 * @file log.h
 * @brief LogBridge C 日志接口。
 * @author Li9htning (Li9htning@hotmail.com)
 * @copyright Copyright 2026 Li9htning.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "../detail/binding_init.h"
#include "../detail/macro_utils.h"
#include "log_impl.h"

#define LOGBRIDGE_LOG(level, message, length)                                \
  LOGBRIDGE_LOG_IMPL(LOGBRIDGE_DETAIL_UNIQUE_NAME(logbridge_macro_binding_), \
                     LOGBRIDGE_DETAIL_UNIQUE_NAME(logbridge_macro_level_),   \
                     LOGBRIDGE_DETAIL_UNIQUE_NAME(logbridge_macro_source_),  \
                     (level), (message), (length))

#define LOGBRIDGE_TRACE(message, length) LOGBRIDGE_LOG(LOGBRIDGE_LEVEL_TRACE, (message), (length))
#define LOGBRIDGE_DEBUG(message, length) LOGBRIDGE_LOG(LOGBRIDGE_LEVEL_DEBUG, (message), (length))
#define LOGBRIDGE_INFO(message, length) LOGBRIDGE_LOG(LOGBRIDGE_LEVEL_INFO, (message), (length))
#define LOGBRIDGE_WARN(message, length) LOGBRIDGE_LOG(LOGBRIDGE_LEVEL_WARN, (message), (length))
#define LOGBRIDGE_ERROR(message, length) LOGBRIDGE_LOG(LOGBRIDGE_LEVEL_ERROR, (message), (length))
#define LOGBRIDGE_FATAL(message, length) LOGBRIDGE_LOG(LOGBRIDGE_LEVEL_FATAL, (message), (length))

#define LOGBRIDGE_INIT(binding) logbridge_detail_initialize_binding((binding));
