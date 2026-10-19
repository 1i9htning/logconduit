#include <cpp-library.h>
#include <protocol-config.h>

#include <cstdio>
#include <cstring>

#define CHECK(condition)                                                   \
  do {                                                                     \
    if (!(condition)) {                                                    \
      std::fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
      return 1;                                                            \
    }                                                                      \
  } while (0)

struct Logger {
  unsigned calls = 0;
  unsigned queries = 0;
  unsigned info_line = 0;
  unsigned debug_calls = 0;
  bool invalid = false;
};

static demo_c_bool_t c_enabled(void* context, demo_c_level_t level) {
  Logger& logger = *static_cast<Logger*>(context);
  ++logger.queries;
  if (level != DEMO_C_LEVEL_DEBUG && level != DEMO_C_LEVEL_INFO) logger.invalid = true;
  return level >= DEMO_C_LEVEL_INFO ? DEMO_C_TRUE : DEMO_C_FALSE;
}

static void c_log(void* context, demo_c_level_t level,
                  const demo_c_source_location_t* source,
                  const char* message, uint32_t length) {
  Logger& logger = *static_cast<Logger*>(context);
  const char expected[4] = {'C', '\0', '9', '!'};
  ++logger.calls;
  if (level != DEMO_C_LEVEL_INFO || length != 4 ||
      std::memcmp(message, expected, 4) != 0 || !source ||
      !source->file_name || !std::strstr(source->file_name, "c-library.c") ||
      !source->function_name || std::strcmp(source->function_name, "demo_c_emit") != 0 ||
      !source->line) {
    logger.invalid = true;
    return;
  }
  logger.info_line = source->line;
}

struct WithoutEnabled {
  static void log(Logger& logger, demo_cpp_level_t level,
                  const demo_cpp_source_location_t& source,
                  const char* message, uint32_t length) noexcept {
    const char expected[5] = {'C', '+', '+', '\0', '!'};
    ++logger.calls;
    if (length != 5 || std::memcmp(message, expected, 5) != 0 ||
        !source.file_name || !std::strstr(source.file_name, "cpp-library.cpp") ||
        !source.function_name || std::strcmp(source.function_name, "demo_cpp_emit") != 0 ||
        !source.line) logger.invalid = true;
    if (level == DEMO_CPP_LEVEL_INFO)
      logger.info_line = source.line;
    else if (level == DEMO_CPP_LEVEL_DEBUG)
      ++logger.debug_calls;
    else
      logger.invalid = true;
  }
};

struct WithEnabled : WithoutEnabled {
  static bool enabled(Logger& logger, demo_cpp_level_t level) noexcept {
    ++logger.queries;
    if (level != DEMO_CPP_LEVEL_DEBUG && level != DEMO_CPP_LEVEL_INFO) logger.invalid = true;
    return level >= DEMO_CPP_LEVEL_INFO;
  }
};

int main(int argc, char** argv) {
  CHECK(argc == 2);
  const bool filtered = std::strcmp(argv[1], "filtered") == 0;
  CHECK(filtered || std::strcmp(argv[1], "unfiltered") == 0);
  Logger c_logger, cpp_logger;
  demo_c_emit();
  demo_cpp_emit();
  CHECK(demo_c_message_evaluations() == 0 && demo_cpp_message_evaluations() == 0);

  demo_c_init_log(&c_logger, c_enabled, c_log);
  if (filtered)
    demo_cpp::initialize<WithEnabled>(cpp_logger);
  else
    demo_cpp::initialize<WithoutEnabled>(cpp_logger);

  const unsigned c_line = demo_c_emit();
  CHECK(c_logger.calls == 1 && c_logger.queries == 2 && !c_logger.invalid);
  CHECK(c_logger.info_line == c_line && cpp_logger.calls == 0 && cpp_logger.queries == 0);
  CHECK(demo_c_message_evaluations() == 1 && demo_cpp_message_evaluations() == 0);

  const unsigned cpp_line = demo_cpp_emit();
  CHECK(!cpp_logger.invalid && cpp_logger.info_line == cpp_line);
  CHECK(cpp_logger.calls == (filtered ? 1u : 2u));
  CHECK(cpp_logger.queries == (filtered ? 2u : 0u));
  CHECK(cpp_logger.debug_calls == (filtered ? 0u : 1u));
  CHECK(demo_cpp_message_evaluations() == (filtered ? 1u : 2u));
  CHECK(c_logger.calls == 1 && c_logger.queries == 2 && !c_logger.invalid);

  // 再次交错输出日志，检测任一初始化覆盖另一 DSO 绑定的情况
  demo_c_emit();
  demo_cpp_emit();
  CHECK(c_logger.calls == 2 && c_logger.queries == 4 && !c_logger.invalid);
  CHECK(cpp_logger.calls == (filtered ? 2u : 4u) && !cpp_logger.invalid);
  return 0;
}
