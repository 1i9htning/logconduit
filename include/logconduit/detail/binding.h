#pragma once

#include <stddef.h>
#include <stdint.h>

#ifndef LOGCONDUIT_PROTOCOL_CONFIG
#error "Define LOGCONDUIT_PROTOCOL_CONFIG to the consuming library's private config header"
#else
#include LOGCONDUIT_PROTOCOL_CONFIG

#endif

/**
 * @brief 针对某一个 binding 进行日志等级可用性检查
 */
static inline logconduit_bool_t logconduit_detail_binding_is_enabled(const logconduit_logger_binding_t* logger_binding, logconduit_level_t level) {
  if (logger_binding->log_callback == NULL) {
    return LOGCONDUIT_FALSE;
  }

  if (logger_binding->enabled_callback == NULL) {
    return LOGCONDUIT_TRUE;
  }

  if (logger_binding->enabled_callback(logger_binding->context, level) == LOGCONDUIT_FALSE) {
    return LOGCONDUIT_FALSE;
  }

  return LOGCONDUIT_TRUE;
}

/**
 * @brief 针对某一个 binding 进行日志的提交
 * @details 因为在规则上，必须先通过 logconduit_detail_binding_is_enabled 再调用 logconduit_detail_binding_emit，因此该函数中无需再进行空指针检查
 */
static inline void logconduit_detail_binding_emit(const logconduit_logger_binding_t* logger_binding, logconduit_level_t level, const logconduit_source_location_t* source, const char* message, uint32_t length) {
  logger_binding->log_callback(logger_binding->context, level, source, message, length);
}
