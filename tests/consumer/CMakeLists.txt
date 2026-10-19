cmake_minimum_required(VERSION 3.22)
project(distributed_consumers LANGUAGES C CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
enable_testing()

# 此处刻意不使用 find_package(logconduit)、SDK 目标、配置宏或私有路径。
add_library(distributed_c SHARED IMPORTED)
set_target_properties(distributed_c PROPERTIES
  IMPORTED_LOCATION "${C_LIBRARY}"
  INTERFACE_INCLUDE_DIRECTORIES "${DISTRIBUTION}/include")
add_library(distributed_cpp SHARED IMPORTED)
set_target_properties(distributed_cpp PROPERTIES
  IMPORTED_LOCATION "${CPP_LIBRARY}"
  INTERFACE_INCLUDE_DIRECTORIES "${DISTRIBUTION}/include")

add_executable(c_consumer c-consumer.c)
set_target_properties(c_consumer PROPERTIES C_STANDARD 99 C_STANDARD_REQUIRED YES C_EXTENSIONS NO)
target_link_libraries(c_consumer PRIVATE distributed_c)
add_test(NAME c_callbacks COMMAND c_consumer)

add_executable(cpp_consumer cpp-consumer.cpp)
set_target_properties(cpp_consumer PROPERTIES CXX_STANDARD 11 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS NO)
target_link_libraries(cpp_consumer PRIVATE distributed_c distributed_cpp)
# 每种 Policy 使用独立进程，确保两个 DSO 均不重复初始化。
add_test(NAME cpp_filtered_and_dso_isolation COMMAND cpp_consumer filtered)
add_test(NAME cpp_without_enabled_and_dso_isolation COMMAND cpp_consumer unfiltered)
