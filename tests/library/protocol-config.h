#ifndef DEMO_C_LIBRARY_H
#define DEMO_C_LIBRARY_H

// [SEG] logconduit:core
#include <stdint.h>

/**
 * @brief ABI 可靠的布尔值
 */
typedef uint32_t demo_c_bool_t;
#define DEMO_C_TRUE ((demo_c_bool_t)1u)
#define DEMO_C_FALSE ((demo_c_bool_t)0u)

/**
 * @brief 日志等级
 */
typedef uint32_t demo_c_level_t;
#define DEMO_C_LEVEL_TRACE ((demo_c_level_t)100u)
#define DEMO_C_LEVEL_DEBUG ((demo_c_level_t)200u)
#define DEMO_C_LEVEL_INFO ((demo_c_level_t)300u)
#define DEMO_C_LEVEL_WARN ((demo_c_level_t)400u)
#define DEMO_C_LEVEL_ERROR ((demo_c_level_t)500u)
#define DEMO_C_LEVEL_FATAL ((demo_c_level_t)600u)

/**
 * @brief 源码位置
 */
typedef struct demo_c_source_location {
  const char* file_name;
  const char* function_name;
  uint32_t line;
} demo_c_source_location_t;

/**
 * @brief 日志器绑定
 */
typedef demo_c_bool_t (*demo_c_enabled_callback_t)(void* context, demo_c_level_t level);
typedef void (*demo_c_log_callback_t)(void* context, demo_c_level_t level, const demo_c_source_location_t* source, const char* message, uint32_t length);
typedef struct demo_c_logger_binding {
  void* context;
  demo_c_enabled_callback_t enabled_callback;
  demo_c_log_callback_t log_callback;
} demo_c_logger_binding_t;
// [ENDSEG] logconduit:core

// [SEG] logconduit:c_adapter
#ifdef __cplusplus
extern "C" {
#endif

__attribute__((visibility("default")))
void demo_c_init_log(void* logger, demo_c_enabled_callback_t enabled, demo_c_log_callback_t callback);

#ifdef __cplusplus
}
#endif
// [ENDSEG] logconduit:c_adapter

#ifdef __cplusplus
extern "C" {
#endif
__attribute__((visibility("default"))) unsigned demo_c_emit(void);
__attribute__((visibility("default"))) unsigned demo_c_message_evaluations(void);
#ifdef __cplusplus
}
#endif

#endif
