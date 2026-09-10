#pragma once

#include <stddef.h>
#include <stdint.h>

/**
 * @brief ABI 可靠的布尔值
 */
typedef uint32_t logconduit_bool_t;
#define LOGCONDUIT_TRUE ((logconduit_bool_t)1u)
#define LOGCONDUIT_FALSE ((logconduit_bool_t)0u)

/**
 * @brief 日志等级
 */
typedef uint32_t logconduit_level_t;
#define LOGCONDUIT_LEVEL_TRACE ((logconduit_level_t)100u)
#define LOGCONDUIT_LEVEL_DEBUG ((logconduit_level_t)200u)
#define LOGCONDUIT_LEVEL_INFO ((logconduit_level_t)300u)
#define LOGCONDUIT_LEVEL_WARN ((logconduit_level_t)400u)
#define LOGCONDUIT_LEVEL_ERROR ((logconduit_level_t)500u)
#define LOGCONDUIT_LEVEL_FATAL ((logconduit_level_t)600u)

/**
 * @brief Source location
 */
typedef struct logconduit_source_location {
  const char* file_name;
  const char* function_name;
  uint32_t line;
} logconduit_source_location_t;

/**
 * @brief Logger binding
 */
typedef logconduit_bool_t (*logconduit_enabled_callback_t)(void* context, logconduit_level_t level);
typedef void (*logconduit_log_callback_t)(void* context, logconduit_level_t level, const logconduit_source_location_t* source, const char* message, uint32_t length);
typedef struct logconduit_logger_binding {
  void* context;
  logconduit_enabled_callback_t enabled_callback;
  logconduit_log_callback_t log_callback;
} logconduit_logger_binding_t;

/**
 * @brief 针对某一个 binding 进行日志等级可用性检查
 */
static inline logconduit_bool_t logconduit_binding_is_enabled(const logconduit_logger_binding_t* logger_binding, logconduit_level_t level) {
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
 * @details 因为在规则上，必须先通过 logconduit_binding_is_enabled 再调用 logconduit_binding_emit，因此该函数中无需再进行空指针检查
 */
static inline void logconduit_binding_emit(const logconduit_logger_binding_t* logger_binding, logconduit_level_t level, const logconduit_source_location_t* source, const char* message, uint32_t length) {
  logger_binding->log_callback(logger_binding->context, level, source, message, length);
}
