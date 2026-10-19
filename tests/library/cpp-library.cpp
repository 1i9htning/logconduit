#include "cpp-library.h"
#include <logconduit/cpp/log.hpp>
#include <string>
#include <type_traits>

#define SAME_TYPE(a, b) static_assert(std::is_same<a, b>::value, "public ABI alias required")
SAME_TYPE(logconduit_bool_t, demo_cpp_bool_t);
SAME_TYPE(logconduit_level_t, demo_cpp_level_t);
SAME_TYPE(logconduit_source_location_t, demo_cpp_source_location_t);
SAME_TYPE(logconduit_enabled_callback_t, demo_cpp_enabled_callback_t);
SAME_TYPE(logconduit_log_callback_t, demo_cpp_log_callback_t);
SAME_TYPE(logconduit_logger_binding_t, demo_cpp_logger_binding_t);

static unsigned evaluations;
static std::string message() {
  const char bytes[5] = {'C', '+', '+', '\0', '!'};
  ++evaluations;
  return std::string(bytes, sizeof(bytes));
}

unsigned demo_cpp_emit(void) {
  LOGCONDUIT_DEBUG(message());
  const unsigned info_line = __LINE__ + 1;
  LOGCONDUIT_INFO(message());
  return info_line;
}

unsigned demo_cpp_message_evaluations(void) { return evaluations; }
