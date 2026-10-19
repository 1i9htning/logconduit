#pragma once

#include "../detail/macro-utils.h"
#include "../detail/storage.h"
#include "detail/message-view.hpp"

#define LOGCONDUIT_LOG_IMPL(local_binding_variable_name,                                                                                                                                           \
                            local_level_variable_name,                                                                                                                                             \
                            local_source_variable_name,                                                                                                                                            \
                            local_message_variable_name,                                                                                                                                           \
                            message_view_variable_name,                                                                                                                                            \
                            level, message)                                                                                                                                                        \
  do {                                                                                                                                                                                             \
    const logconduit_logger_binding_t* local_binding_variable_name = logconduit_detail_logger_binding();                                                                                           \
    logconduit_level_t local_level_variable_name = (level);                                                                                                                                        \
    if (logconduit_detail_binding_is_enabled(local_binding_variable_name, local_level_variable_name) == LOGCONDUIT_TRUE) {                                                                         \
      const auto& local_message_variable_name = (message);                                                                                                                                         \
      auto message_view_variable_name = logconduit::detail::make_message_view(local_message_variable_name);                                                                                        \
      logconduit_source_location_t local_source_variable_name = {__FILE__, __func__, __LINE__};                                                                                                    \
      logconduit_detail_binding_emit(local_binding_variable_name, local_level_variable_name, &local_source_variable_name, (message_view_variable_name.data), (message_view_variable_name.length)); \
    }                                                                                                                                                                                              \
  } while (0)

#define LOGCONDUIT_LOG(level, message)                                               \
  LOGCONDUIT_LOG_IMPL(LOGCONDUIT_DETAIL_UNIQUE_NAME(logconduit_macro_binding_),      \
                      LOGCONDUIT_DETAIL_UNIQUE_NAME(logconduit_macro_level_),        \
                      LOGCONDUIT_DETAIL_UNIQUE_NAME(logconduit_macro_source_),       \
                      LOGCONDUIT_DETAIL_UNIQUE_NAME(logconduit_macro_message_),      \
                      LOGCONDUIT_DETAIL_UNIQUE_NAME(logconduit_macro_message_view_), \
                      (level), (message))

#define LOGCONDUIT_TRACE(message) LOGCONDUIT_LOG(LOGCONDUIT_LEVEL_TRACE, message)
#define LOGCONDUIT_DEBUG(message) LOGCONDUIT_LOG(LOGCONDUIT_LEVEL_DEBUG, message)
#define LOGCONDUIT_INFO(message) LOGCONDUIT_LOG(LOGCONDUIT_LEVEL_INFO, message)
#define LOGCONDUIT_WARN(message) LOGCONDUIT_LOG(LOGCONDUIT_LEVEL_WARN, message)
#define LOGCONDUIT_ERROR(message) LOGCONDUIT_LOG(LOGCONDUIT_LEVEL_ERROR, message)
#define LOGCONDUIT_FATAL(message) LOGCONDUIT_LOG(LOGCONDUIT_LEVEL_FATAL, message)
