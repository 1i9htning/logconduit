# Logconduit

> 面向 C/C++ 动态库的轻量级 Header-Only 日志桥接库，用于将库内部日志高效转发至宿主应用提供的 Logger。

```text
Dynamic Library
      ↓
   Logconduit
      ↓
Developer Logger
```

Logconduit 不是日志框架，不负责日志输出、格式化、异步线程、轮转、Logger 管理等功能。

## 支持

- C
- C++11 及以上
- Header-Only
- 当前仅支持 GCC / Clang + ELF

日志级别：

```text
TRACE = 100
DEBUG = 200
INFO  = 300
WARN  = 400
ERROR = 500
FATAL = 600
```

宿主 Logger 的 Level 模型可以不同，由 Adapter 自行完成映射。

## C

```c
#include <logconduit/c/log.h>
```

初始化：

```c
static logconduit_bool_t enabled(
    void* context,
    logconduit_level_t level) {
  /* 判断对应 Level 是否启用 */
}

static void log(
    void* context,
    logconduit_level_t level,
    const logconduit_source_location_t* source,
    const char* message,
    uint32_t length) {
  /* 转发给宿主 Logger */
}

logconduit_logger_binding_t binding = {
    &logger,
    enabled,
    log,
};

LOGCONDUIT_INIT(binding);
```

写日志：

```c
LOGCONDUIT_INFO("hello", 5);
LOGCONDUIT_ERROR(message, length);
```

C Frontend 使用 `const char* + uint32_t` 表示消息，消息不要求以 `'\0'` 结尾。

## C++

```cpp
#include <logconduit/cpp/log.hpp>
```

C++ Frontend 支持自动获取消息长度：

```cpp
LOGCONDUIT_INFO("hello");

std::string message = "hello";
LOGCONDUIT_INFO(message);
```

C++17 及以上支持：

```cpp
std::string_view message = "hello";
LOGCONDUIT_INFO(message);
```

裸 `const char*` 不受支持，因为 Logconduit 不会隐式执行 `strlen()`。

### Logger Adapter

为具体 Logger 定义 Policy：

```cpp
struct MyLoggerPolicy {
  static bool enabled(
      MyLogger& logger,
      logconduit_level_t level) noexcept {
    /* Level 映射与 enabled 判断 */
  }

  static void log(
      MyLogger& logger,
      logconduit_level_t level,
      const logconduit_source_location_t& source,
      const char* message,
      uint32_t length) noexcept {
    /* Level 映射并调用 Logger */
  }
};
```

初始化：

```cpp
MyLogger logger;
logconduit::initialize<MyLoggerPolicy>(logger);
```

如果 Logger 不支持 enabled 查询，可以省略 `Policy::enabled()`，此时所有 Level 均视为启用。

## 注意事项

- 每个动态库生命周期内最多初始化一次 Binding。
- Logconduit 不拥有 Logger，Logger 必须在动态库仍可能产生日志期间保持有效。
- Callback 可能被多个线程并发调用，线程安全由宿主 Logger / Adapter 保证。
- Logconduit 不复制消息；异步 Logger 必须在 Callback 返回前自行复制。
- Source Location 包含文件名、函数名和行号。
- C++ Frontend 单条消息长度必须不超过 `UINT32_MAX`：
  - Debug 构建会通过断言检查。
  - Release 构建不增加额外检查，超出限制属于违反前置条件。
- C++ Adapter Callback 为 `noexcept`，异常不得穿越 Callback 边界。
