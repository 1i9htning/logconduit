#pragma once

#include "../core.h"

#define LOGBRIDGE_LOG_IMPL(local_binding_variable_name, local_level_variable_name, local_source_variable_name, level, message, length)  \
  do {                                                                                                                                  \
    const logbridge_logger_binding_t* local_binding_variable_name = logbridge_detail_logger_binding();                                  \
    logbridge_level_t local_level_variable_name = (level);                                                                              \
    if (logbridge_binding_is_enabled(local_binding_variable_name, local_level_variable_name) == LOGBRIDGE_TRUE) {                       \
      logbridge_source_location_t local_source_variable_name = {__FILE__, __func__, __LINE__};                                          \
      logbridge_binding_emit(local_binding_variable_name, local_level_variable_name, &local_source_variable_name, (message), (length)); \
    }                                                                                                                                   \
  } while (0)
