#pragma once

#if defined(_MSVC_LANG)
#define LOGCONDUIT_CPP_STANDARD _MSVC_LANG
#else
#define LOGCONDUIT_CPP_STANDARD __cplusplus
#endif