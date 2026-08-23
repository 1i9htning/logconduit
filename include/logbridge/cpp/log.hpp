/**
 * @file log.hpp
 * @brief LogBridge C++ 日志接口。
 * @author Li9htning (Li9htning@hotmail.com)
 * @copyright Copyright 2026 Li9htning.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#pragma once

#include "../core.h"
#include "../detail/macro_utils.h"
#include "binding.hpp"
#include "log_impl.hpp"

#define LOGBRIDGE_LOG(level, message)                                             \
  LOGBRIDGE_LOG_IMPL(LOGBRIDGE_DETAIL_UNIQUE_NAME(logbridge_macro_binding_),      \
                     LOGBRIDGE_DETAIL_UNIQUE_NAME(logbridge_macro_level_),        \
                     LOGBRIDGE_DETAIL_UNIQUE_NAME(logbridge_macro_source_),       \
                     LOGBRIDGE_DETAIL_UNIQUE_NAME(logbridge_macro_message_),      \
                     LOGBRIDGE_DETAIL_UNIQUE_NAME(logbridge_macro_message_view_), \
                     (level), (message))

#define LOGBRIDGE_TRACE(message) LOGBRIDGE_LOG(LOGBRIDGE_LEVEL_TRACE, message)
#define LOGBRIDGE_DEBUG(message) LOGBRIDGE_LOG(LOGBRIDGE_LEVEL_DEBUG, message)
#define LOGBRIDGE_INFO(message) LOGBRIDGE_LOG(LOGBRIDGE_LEVEL_INFO, message)
#define LOGBRIDGE_WARN(message) LOGBRIDGE_LOG(LOGBRIDGE_LEVEL_WARN, message)
#define LOGBRIDGE_ERROR(message) LOGBRIDGE_LOG(LOGBRIDGE_LEVEL_ERROR, message)
#define LOGBRIDGE_FATAL(message) LOGBRIDGE_LOG(LOGBRIDGE_LEVEL_FATAL, message)
