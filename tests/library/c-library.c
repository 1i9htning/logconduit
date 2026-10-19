#include <logconduit/c/log.h>

#include "protocol-config.h"

// 验证类型兼容性，而非仅大小相等：不能重造私有 ABI
#define SAME_TYPE(a, b) typedef char same_##a[__builtin_types_compatible_p(a, b) ? 1 : -1]
SAME_TYPE(logconduit_bool_t, demo_c_bool_t);
SAME_TYPE(logconduit_level_t, demo_c_level_t);
SAME_TYPE(logconduit_source_location_t, demo_c_source_location_t);
SAME_TYPE(logconduit_enabled_callback_t, demo_c_enabled_callback_t);
SAME_TYPE(logconduit_log_callback_t, demo_c_log_callback_t);
SAME_TYPE(logconduit_logger_binding_t, demo_c_logger_binding_t);

static unsigned evaluations;
static const char* message(void) {
  // 消息含嵌入 NUL，但不以 NUL 结尾
  static const char bytes[4] = {'C', '\0', '9', '!'};
  ++evaluations;
  return bytes;
}

unsigned demo_c_emit(void) {
  unsigned info_line;
  LOGCONDUIT_DEBUG(message(), 4);
  info_line = __LINE__ + 1;
  LOGCONDUIT_INFO(message(), 4);
  return info_line;
}

unsigned demo_c_message_evaluations(void) { return evaluations; }
