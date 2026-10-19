#include <protocol-config.h>
#include <stdio.h>
#include <string.h>

#define CHECK(condition)                                              \
  do {                                                                \
    if (!(condition)) {                                               \
      fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
      return 1;                                                       \
    }                                                                 \
  } while (0)

struct logger {
  unsigned calls;
  unsigned queries;
  unsigned line;
  int invalid;
};

static demo_c_bool_t enabled(void* context, demo_c_level_t level) {
  struct logger* logger = (struct logger*)context;
  ++logger->queries;
  if (level != DEMO_C_LEVEL_DEBUG && level != DEMO_C_LEVEL_INFO) logger->invalid = 1;
  return level >= DEMO_C_LEVEL_INFO ? DEMO_C_TRUE : DEMO_C_FALSE;
}

static void log_message(void* context, demo_c_level_t level,
                        const demo_c_source_location_t* source,
                        const char* message, uint32_t length) {
  const char expected[4] = {'C', '\0', '9', '!'};
  struct logger* logger = (struct logger*)context;
  ++logger->calls;
  if (level != DEMO_C_LEVEL_INFO || length != 4 ||
      memcmp(message, expected, 4) != 0 || !source ||
      !source->file_name || !strstr(source->file_name, "c-library.c") ||
      !source->function_name || strcmp(source->function_name, "demo_c_emit") != 0 ||
      source->line == 0) {
    logger->invalid = 1;
    return;
  }
  logger->line = source->line;
}

int main(void) {
  struct logger logger = {0, 0, 0, 0};
  unsigned line;
  demo_c_emit();  // 未初始化的绑定可安全使用，并跳过消息求值
  CHECK(demo_c_message_evaluations() == 0);
  demo_c_init_log(&logger, enabled, log_message);
  line = demo_c_emit();
  CHECK(logger.calls == 1 && logger.queries == 2 && !logger.invalid);
  CHECK(logger.line == line);
  CHECK(demo_c_message_evaluations() == 1);
  return 0;
}
