#pragma once

#if defined(__clang__) || defined(__GNUC__)
#ifdef __ELF__
#define LOGBRIDGE_DETAIL_PER_DSO_STORAGE __attribute__((weak, visibility("hidden")))
#else
#error "Unsupported object format"
#endif
#else
#error "Unsupported compiler"
#endif
