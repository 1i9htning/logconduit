function(run_checked)
  execute_process(COMMAND ${ARGV} RESULT_VARIABLE result)
  if(NOT result EQUAL 0)
    message(FATAL_ERROR "Consumer command failed (${result}): ${ARGV}")
  endif()
endfunction()

if(NOT BUILD_CONFIG)
  set(BUILD_CONFIG Debug)
endif()

run_checked("${CMAKE_COMMAND}" -S "${SOURCE_DIR}" -B "${BINARY_DIR}"
  "-DDISTRIBUTION=${DISTRIBUTION}" "-DC_LIBRARY=${C_LIBRARY}" "-DCPP_LIBRARY=${CPP_LIBRARY}"
  "-DCMAKE_C_COMPILER=${C_COMPILER}" "-DCMAKE_CXX_COMPILER=${CXX_COMPILER}"
  "-DCMAKE_BUILD_TYPE=${BUILD_CONFIG}" "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON")

# 独立消费者不得通过传递依赖获得 SDK 头文件路径、私有生成路径，
# 也不得获得 SDK 的协议选择宏。
file(READ "${BINARY_DIR}/compile_commands.json" commands)
foreach(forbidden IN ITEMS "${SDK_SOURCE}/include" "${SDK_SOURCE}/tests/library"
    "${SDK_SOURCE}/src" "LOGCONDUIT_")
  string(FIND "${commands}" "${forbidden}" position)
  if(NOT position EQUAL -1)
    message(FATAL_ERROR "Consumer compile command leaks private dependency: ${forbidden}")
  endif()
endforeach()

run_checked("${CMAKE_COMMAND}" --build "${BINARY_DIR}" --config "${BUILD_CONFIG}")
run_checked("${CMAKE_CTEST_COMMAND}" --test-dir "${BINARY_DIR}"
  -C "${BUILD_CONFIG}" --output-on-failure)
