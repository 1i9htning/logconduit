cmake_minimum_required(VERSION 3.22)
project(logconduit_tests LANGUAGES C CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
enable_testing()

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/.." "${CMAKE_CURRENT_BINARY_DIR}/sdk")
get_target_property(sdk_source logconduit::logconduit _LOGCONDUIT_SOURCE_DIR)
get_filename_component(sdk_source "${sdk_source}" REALPATH)

# 这些库模拟 SDK 使用者：仅在公共头中放置标记。
add_library(demo_c SHARED library/c-library.c)
set_target_properties(demo_c PROPERTIES C_STANDARD 99 C_STANDARD_REQUIRED YES C_EXTENSIONS NO
  C_VISIBILITY_PRESET hidden)
target_include_directories(demo_c PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/library")
# protocol-config.h 是合法的公共头名，不能遮蔽任一 DSO 的私有配置。
logconduit_configure_target(
  TARGET demo_c PUBLIC_HEADER protocol-config.h
  PUBLIC_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/library"
  TYPE_PREFIX demo_c MACRO_PREFIX DEMO_C ADAPTER C)

add_library(demo_cpp SHARED library/cpp-library.cpp)
set_target_properties(demo_cpp PROPERTIES CXX_STANDARD 11 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO
  CXX_VISIBILITY_PRESET hidden VISIBILITY_INLINES_HIDDEN YES)
target_include_directories(demo_cpp PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/library")
logconduit_configure_target(
  TARGET demo_cpp PUBLIC_HEADER cpp-library.h
  PUBLIC_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/library"
  TYPE_PREFIX demo_cpp MACRO_PREFIX DEMO_CPP ADAPTER CPP)

# 独立消费者仅使用分发副本，不接触 SDK 或库源码中的头文件。
set(distribution "${CMAKE_CURRENT_BINARY_DIR}/distribution")
file(MAKE_DIRECTORY "${distribution}/include" "${distribution}/lib")
configure_file(library/protocol-config.h "${distribution}/include/protocol-config.h" COPYONLY)
configure_file(library/cpp-library.h "${distribution}/include/cpp-library.h" COPYONLY)
foreach(library IN ITEMS demo_c demo_cpp)
  add_custom_command(TARGET ${library} POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
      "$<TARGET_FILE:${library}>" "${distribution}/lib/$<TARGET_FILE_NAME:${library}>"
    VERBATIM)
endforeach()

add_test(NAME logconduit.distributed_consumers
  COMMAND "${CMAKE_COMMAND}"
    "-DSOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}/consumer"
    "-DBINARY_DIR=${CMAKE_CURRENT_BINARY_DIR}/consumer-build/$<CONFIG>"
    "-DDISTRIBUTION=${distribution}"
    "-DC_LIBRARY=${distribution}/lib/$<TARGET_FILE_NAME:demo_c>"
    "-DCPP_LIBRARY=${distribution}/lib/$<TARGET_FILE_NAME:demo_cpp>"
    "-DC_COMPILER=${CMAKE_C_COMPILER}"
    "-DCXX_COMPILER=${CMAKE_CXX_COMPILER}"
    "-DBUILD_CONFIG=$<CONFIG>"
    "-DSDK_SOURCE=${sdk_source}"
    -P "${CMAKE_CURRENT_SOURCE_DIR}/run-consumers.cmake")
