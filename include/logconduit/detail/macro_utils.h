#pragma once

/**
 * @brief 利用 __COUNTER__ 生成唯一的内部标识符，降低宏展开后与用户标识符发生名称冲突的可能性。
 */
#define LOGCONDUIT_DETAIL_CONCAT_IMPL(a, b) a##b
#define LOGCONDUIT_DETAIL_CONCAT(a, b) LOGCONDUIT_DETAIL_CONCAT_IMPL(a, b)
#define LOGCONDUIT_DETAIL_UNIQUE_NAME(name) LOGCONDUIT_DETAIL_CONCAT(name, __COUNTER__)