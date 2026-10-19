![Logo](./logo.svg)

[简体中文](./README.md) | English

A lightweight logging bridge SDK for C/C++ shared libraries that forwards internal library logs to the host application's logger. It is not a logging framework: it does not handle output, formatting, asynchronous logging threads, log rotation, or logger management.

Currently supports C99 / C++11 and later, with GCC / Clang + ELF.

Inside the library, logging uses a header-only frontend. CMake embeds the public protocol and the selected initialization adapter in the library's public header, and compiles the adapter implementation into the shared library. **End users need only the library's own public header and shared library, with no Logconduit headers, configuration macros, or private generated directory.**

## Developing Logconduit

This project configures clangd to use C++17 by default for IDE analysis. This does not change the CMake compilation standard or raise the minimum compatibility requirement beyond C99 / C++11.

## Building and Running Tests Independently

The root project only configures the SDK; it enables no compilation languages and does not load tests. The tests are a standalone CMake project. Run the following commands from the repository root, or use the corresponding absolute paths when working elsewhere:

```sh
cmake -S tests -B build/tests
cmake --build build/tests
ctest --test-dir build/tests --output-on-failure
```

## Library Developers: Add Only the Fixed Markers to the Public Header

Add the SDK source as a Git submodule and call `add_subdirectory` directly from the library project. This does not load `tests/`.

The SDK's root project uses `LANGUAGES NONE`, so configuring the SDK itself does not require a compiler; `logconduit::logconduit` is an INTERFACE target. Templates reside in the SDK's `template/`, and private files are generated directly in the SDK's `src/`, with no subdirectories. **The SDK's `src/` directory, or the parent directory needed to create it, must be writable**; a read-only source copy cannot be used with this generation workflow. The library's public header must also be writable, because its marked regions are rewritten during configuration.

For example, a C library's `include/a.h`:

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

The markers must be at global scope, not enclosed in an outer `extern "C"` block. `core` includes `stdint.h` itself, and the C adapter supplies its own C linkage. During configuration, CMake updates only the selected regions, preserving library APIs outside those regions. The public header does not need any `logconduit` includes.

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

Do not pass `CONFIG_HEADER`; there is also no need to call embedding functions manually or add adapter source files. The file stem is derived from `TYPE_PREFIX` by converting it to lowercase and replacing `_` with `-`; prefixes in the code remain unchanged. The main configuration entry point generates files directly in the SDK's `src/`: `<stem>-protocol-config.h` and the selected `<stem>-c-adapter.c` or `<stem>-cpp-adapter.cpp`. These are automatically added to the target. The C example above generates `src/a-log-protocol-config.h` and `src/a-log-c-adapter.c`. The SDK include paths, the protocol-selection macro containing an absolute path, the public-header reference, and the private generated directory are private to the library target and do not propagate to the application. Private protocol types are aliases of the public ABI types, not recreated structures.

Output files are named using the normalized file stem, with no automatically isolated subdirectories. Different libraries sharing one SDK copy must use prefixes that do not produce the same stem. Do not reuse the same output file for different protocols or concurrent configurations; when independent configurations are needed, use prefixes with different stems or separate SDK copies.

Inside the library, `src/a.c`:

```c
#include "a.h"
#include <logconduit/c/log.h>

void A_example(void) {
  LOGCONDUIT_INFO("hello", 5);
}
```

The library author distributes the generated `a.h` and the shared library; the SDK does not handle distribution of the library's APIs. Messages use `const char* + uint32_t`; they do not need to be NUL-terminated and may contain embedded NULs.

### C Applications: Use Only a.h and the Shared Library

The generated entry point is `a_log_init_log`; do not call the private `LOGCONDUIT_INIT` in the application:

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

## C++ Libraries and Applications

A C++ library's public header uses `core` + `cpp_adapter`; **no C adapter section is needed**:

```cpp
#pragma once

// [SEG] logconduit:core
// [ENDSEG] logconduit:core

// [SEG] logconduit:cpp_adapter
// [ENDSEG] logconduit:cpp_adapter

void B_example();
```

The CMake setup is the same as for the C library, but uses `project(B LANGUAGES CXX)`, C++ source files, the C++11 standard, and the following configuration:

```cmake
logconduit_configure_target(
  TARGET B PUBLIC_HEADER b.h
  PUBLIC_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include"
  TYPE_PREFIX b_log MACRO_PREFIX B_LOG ADAPTER CPP)
```

Inside the library, use `<logconduit/cpp/log.hpp>`, for example `LOGCONDUIT_INFO("hello")` or `LOGCONDUIT_INFO(std::string("hello"))`. C++17 and later also support `std::string_view`. A bare `const char*` is not supported; `strlen()` is never called implicitly.

The application includes only `b.h` and defines its own Logger and Policy:

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

`Policy::enabled` is optional; when omitted, all log levels are enabled. C++ supports initialization only through `b_log::initialize<Policy>(logger)` and does not expose an `initialize(binding)` overload. Initialization across translation units is handled by the generated private bridge.

The C and C++ adapters are independent choices: each generates only `core` + the selected section and its corresponding implementation. The log levels are TRACE=100, DEBUG=200, INFO=300, WARN=400, ERROR=500, and FATAL=600; the host can map them to its own levels in a policy / callback.

## Runtime Constraints

- Initialize a binding at most once during each shared library's lifetime, before concurrent logging begins; hot-swapping and concurrent reinitialization are not supported;
- No logs are emitted before initialization. The binding belongs to the shared library's own binary image; do not create separate private SDK storage in the application to initialize it;
- The SDK does not own the logger; the logger and any callback dependencies must remain valid for as long as the shared library may still produce logs;
- Callbacks may be invoked concurrently by multiple threads; the host application's logger / adapter is responsible for thread safety;
- Messages are not copied; asynchronous loggers must copy the message and any required metadata before the callback returns;
- Source locations contain the file name, function name, and line number;
- A single message passed to the C++ frontend must not exceed `UINT32_MAX` in length: Debug builds assert this, while Release builds add no extra checks. Exceeding this length violates a precondition;
- C++ adapter callbacks are `noexcept`; exceptions must not cross the callback boundary.
