![Logo](./logo.svg)

面向 C/C++ 动态库的轻量日志桥接 SDK，将库内部日志转发给宿主 Logger。它不是日志框架，不负责输出、格式化、异步线程、轮转或 Logger 管理。

目前支持 C99 / C++11 及以上，GCC / Clang + ELF。

库内部使用 Header-Only 日志前端；CMake 将公共协议及选定的初始化 Adapter 写入库的公共头，并将 Adapter 实现编入动态库。**最终用户只需要该库自己的公共头与动态库，不需要 Logconduit 头文件、配置宏或私有生成目录。**

## 自身开发

该项目中 clangd 默认使用 C++17 进行 IDE 分析；这不改变 CMake 编译标准，也不提高 C99 / C++11 的最低兼容要求。

## 独立构建测试

根工程只配置 SDK，不启用编译语言，也不加载测试。测试是独立 CMake 工程；以下命令从仓库根目录执行，其他工作目录可使用对应的绝对路径：

```sh
cmake -S tests -B build/tests
cmake --build build/tests
ctest --test-dir build/tests --output-on-failure
```

## 库开发者：公共头只放固定标记

SDK 通过 git submodule 加入源码，库工程直接 `add_subdirectory`，不会加载 `tests/`。

根 SDK 使用 `LANGUAGES NONE`，配置自身不需要编译器；`logconduit::logconduit` 是 INTERFACE 目标。模板位于 SDK 的 `template/`，私有文件平铺生成于 SDK 的 `src/`。**SDK 的 `src/` 或用于创建它的父目录必须可写**；只读源码副本不适用于此生成流程。库的公共头也必须可写，因为配置阶段会回写标记区域。

例如 C 库 `include/a.h`：

```c
#pragma once

// [SEG] logconduit:core
// [ENDSEG] logconduit:core

// [SEG] logconduit:c_adapter
// [ENDSEG] logconduit:c_adapter

#ifdef __cplusplus
extern "C" {
#endif
void A_example(void);
#ifdef __cplusplus
}
#endif
```

标记必须在全局作用域，不能包在外层 `extern "C"` 中。`core` 自含 `stdint.h`，C Adapter 自带 C linkage。CMake 在配置时只更新所选区域，保留区域外的业务 API；公共头无需任何 `logconduit` include。

```cmake
cmake_minimum_required(VERSION 3.22)
project(A LANGUAGES C)
add_subdirectory(third_party/logconduit)

add_library(A SHARED src/a.c)
target_include_directories(A PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
set_target_properties(A PROPERTIES C_STANDARD 99 C_STANDARD_REQUIRED YES)
logconduit_configure_target(
  TARGET A
  PUBLIC_HEADER a.h
  PUBLIC_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include"
  TYPE_PREFIX a_log MACRO_PREFIX A_LOG
  ADAPTER C)
```

不传 `CONFIG_HEADER`，也不需手动调用嵌入函数或添加 Adapter 源码。文件 stem 由 `TYPE_PREFIX` 转为小写并将 `_` 换成 `-`，代码中的前缀保持不变。主入口在 SDK 的 `src/` 直接生成 `<stem>-protocol-config.h`，以及所选的 `<stem>-c-adapter.c` 或 `<stem>-cpp-adapter.cpp`，自动加入目标。以上 C 示例生成 `src/a-log-protocol-config.h` 和 `src/a-log-c-adapter.c`。SDK include、绝对路径的协议选择宏、公共头引用和私有生成目录仅用于库目标，不传播到应用。私有协议类型是公共 ABI 的别名，不是重造结构。

输出以规范化后的文件 stem 命名，没有自动隔离子目录；共享一份 SDK 的不同库必须使用不会产生相同 stem 的前缀。不要让不同协议或并行配置复用同一输出文件；需要独立配置时使用不同 stem 的前缀或独立 SDK 副本。

库内部 `src/a.c`：

```c
#include "a.h"
#include <logconduit/c/log.h>

void A_example(void) {
  LOGCONDUIT_INFO("hello", 5);
}
```

库作者自行分发生成后的 `a.h` 与动态库；SDK 不负责分发库的业务接口。消息使用 `const char* + uint32_t`，不要求 NUL 终止，允许嵌入 NUL。

### C 应用：仅消费 a.h 与动态库

生成的入口是 `a_log_init_log`，不在应用中调用私有 `LOGCONDUIT_INIT`：

```c
#include <a.h>
#include <stdio.h>

static a_log_bool_t enabled(void* logger, a_log_level_t level) {
  (void)logger;
  return level >= A_LOG_LEVEL_INFO ? A_LOG_TRUE : A_LOG_FALSE;
}

static void log_message(void* logger, a_log_level_t level,
                        const a_log_source_location_t* source,
                        const char* message, uint32_t length) {
  (void)level;
  (void)source;
  fwrite(message, 1, length, (FILE*)logger);
}

int main(void) {
  a_log_init_log(stdout, enabled, log_message);
  A_example();
  return 0;
}
```

## C++ 库与应用

C++ 库公共头使用 `core` + `cpp_adapter`，**不需要 C Adapter 段**：

```cpp
#pragma once

// [SEG] logconduit:core
// [ENDSEG] logconduit:core

// [SEG] logconduit:cpp_adapter
// [ENDSEG] logconduit:cpp_adapter

void B_example();
```

CMake 与 C 库相同，但使用 `project(B LANGUAGES CXX)`、C++ 源文件、C++11 标准，并配置：

```cmake
logconduit_configure_target(
  TARGET B PUBLIC_HEADER b.h
  PUBLIC_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include"
  TYPE_PREFIX b_log MACRO_PREFIX B_LOG ADAPTER CPP)
```

库内部使用 `<logconduit/cpp/log.hpp>`，例如 `LOGCONDUIT_INFO("hello")` 或 `LOGCONDUIT_INFO(std::string("hello"))`；C++17 及以上也支持 `std::string_view`。裸 `const char*` 不受支持，不会隐式执行 `strlen()`。

应用只 include `b.h`，定义自己的 Logger 与 Policy：

```cpp
#include <b.h>
#include <cstdio>

struct Logger { std::FILE* output; };
struct Policy {
  static bool enabled(Logger&, b_log_level_t level) noexcept {
    return level >= B_LOG_LEVEL_INFO;
  }
  static void log(Logger& logger, b_log_level_t,
                  const b_log_source_location_t&,
                  const char* message, uint32_t length) noexcept {
    std::fwrite(message, 1, length, logger.output);
  }
};

int main() {
  Logger logger = {stdout};
  b_log::initialize<Policy>(logger);
  B_example();
}
```

`Policy::enabled` 可省略，此时所有 Level 均启用。C++ 只支持 `b_log::initialize<Policy>(logger)` 初始化，不提供公开的 `initialize(binding)` 重载；跨翻译单元初始化由生成的私有 bridge 完成。

C 与 C++ Adapter 是独立选项，各自只生成 `core` + 所选段和对应实现。日志级别为 TRACE=100、DEBUG=200、INFO=300、WARN=400、ERROR=500、FATAL=600；宿主可在 Policy / Callback 中映射为自己的级别。

## 运行时约束

- 每个动态库生命周期内最多初始化一次 Binding，在开始并发记录日志前完成；不支持热切换或并发重新初始化；
- 未初始化时日志不输出。绑定属于动态库自己的 image，不应在应用中另建一份私有 SDK storage 来初始化；
- 不拥有 Logger；Logger 和回调依赖必须在动态库仍可能产生日志期间保持有效；
- Callback 可能被多线程并发调用，线程安全由宿主 Logger / Adapter 保证；
- 不复制消息；异步 Logger 必须在 Callback 返回前自行复制消息及所需 metadata；
- Source Location 包含文件名、函数名和行号；
- C++ Frontend 单条消息长度不得超过 `UINT32_MAX`：Debug 有断言，Release 不增加额外检查，超长违反前置条件；
- C++ Adapter Callback 为 `noexcept`，异常不得穿越回调边界。
