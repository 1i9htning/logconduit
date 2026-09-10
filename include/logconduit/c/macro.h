#pragma once

#include "../core.h"
#include "../detail/binding_init.h"
#include "../detail/macro_utils.h"

#define LOGCONDUIT_LOG_IMPL(local_binding_variable_name, local_level_variable_name, local_source_variable_name, level, message, length)  \
  do {                                                                                                                                   \
    const logconduit_logger_binding_t* local_binding_variable_name = logconduit_detail_logger_binding();                                 \
    logconduit_level_t local_level_variable_name = (level);                                                                              \
    if (logconduit_binding_is_enabled(local_binding_variable_name, local_level_variable_name) == LOGCONDUIT_TRUE) {                      \
      logconduit_source_location_t local_source_variable_name = {__FILE__, __func__, __LINE__};                                          \
      logconduit_binding_emit(local_binding_variable_name, local_level_variable_name, &local_source_variable_name, (message), (length)); \
    }                                                                                                                                    \
  } while (0)

#define LOGCONDUIT_LOG(level, message, length)                                  \
  LOGCONDUIT_LOG_IMPL(LOGCONDUIT_DETAIL_UNIQUE_NAME(logconduit_macro_binding_), \
                      LOGCONDUIT_DETAIL_UNIQUE_NAME(logconduit_macro_level_),   \
                      LOGCONDUIT_DETAIL_UNIQUE_NAME(logconduit_macro_source_),  \
                      (level), (message), (length))

#define LOGCONDUIT_TRACE(message, length) LOGCONDUIT_LOG(LOGCONDUIT_LEVEL_TRACE, (message), (length))
#define LOGCONDUIT_DEBUG(message, length) LOGCONDUIT_LOG(LOGCONDUIT_LEVEL_DEBUG, (message), (length))
#define LOGCONDUIT_INFO(message, length) LOGCONDUIT_LOG(LOGCONDUIT_LEVEL_INFO, (message), (length))
#define LOGCONDUIT_WARN(message, length) LOGCONDUIT_LOG(LOGCONDUIT_LEVEL_WARN, (message), (length))
#define LOGCONDUIT_ERROR(message, length) LOGCONDUIT_LOG(LOGCONDUIT_LEVEL_ERROR, (message), (length))
#define LOGCONDUIT_FATAL(message, length) LOGCONDUIT_LOG(LOGCONDUIT_LEVEL_FATAL, (message), (length))

#define LOGCONDUIT_INIT(binding) logconduit_detail_initialize_binding((binding));
