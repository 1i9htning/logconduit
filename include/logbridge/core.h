#pragma once

#include <stddef.h>
#include <stdint.h>

/**
 * @brief ABI 可靠的布尔值
 */
typedef uint32_t logbridge_bool_t;
#define LOGBRIDGE_TRUE ((logbridge_bool_t)1u)
#define LOGBRIDGE_FALSE ((logbridge_bool_t)0u)

/**
 * @brief 日志等级
 */
typedef uint32_t logbridge_level_t;
#define LOGBRIDGE_LEVEL_TRACE ((logbridge_level_t)100u)
#define LOGBRIDGE_LEVEL_DEBUG ((logbridge_level_t)200u)
#define LOGBRIDGE_LEVEL_INFO ((logbridge_level_t)300u)
#define LOGBRIDGE_LEVEL_WARN ((logbridge_level_t)400u)
#define LOGBRIDGE_LEVEL_ERROR ((logbridge_level_t)500u)
#define LOGBRIDGE_LEVEL_FATAL ((logbridge_level_t)600u)

/**
 * @brief Source location
 */
typedef struct logbridge_source_location {
  const char* file_name;
  const char* function_name;
  uint32_t line;
} logbridge_source_location_t;

/**
 * @brief Logger binding
 */
typedef logbridge_bool_t (*logbridge_enabled_callback_t)(void* context, logbridge_level_t level);
typedef void (*logbridge_log_callback_t)(void* context, logbridge_level_t level, const logbridge_source_location_t* source, const char* message, uint32_t length);
typedef struct logbridge_logger_binding {
  void* context;
  logbridge_enabled_callback_t enabled_callback;
  logbridge_log_callback_t log_callback;
} logbridge_logger_binding_t;

/**
 * @brief 针对某一个 binding 进行日志等级可用性检查
 */
static inline logbridge_bool_t logbridge_binding_is_enabled(const logbridge_logger_binding_t* logger_binding, logbridge_level_t level) {
  if (logger_binding->log_callback == NULL) {
    return LOGBRIDGE_FALSE;
  }

  if (logger_binding->enabled_callback == NULL) {
    return LOGBRIDGE_TRUE;
  }

  if (logger_binding->enabled_callback(logger_binding->context, level) == LOGBRIDGE_FALSE) {
    return LOGBRIDGE_FALSE;
  }

  return LOGBRIDGE_TRUE;
}

/**
 * @brief 针对某一个 binding 进行日志的提交
 * @details 因为在规则上，必须先通过 logbridge_binding_is_enabled 再调用 logbridge_binding_emit，因此该函数中无需再进行空指针检查
 */
static inline void logbridge_binding_emit(const logbridge_logger_binding_t* logger_binding, logbridge_level_t level, const logbridge_source_location_t* source, const char* message, uint32_t length) {
  logger_binding->log_callback(logger_binding->context, level, source, message, length);
}
