cmake_minimum_required(VERSION 3.22)
project(logconduit VERSION 0.1.0 LANGUAGES NONE)

add_library(logconduit INTERFACE)
add_library(logconduit::logconduit ALIAS logconduit)
target_include_directories(logconduit INTERFACE
  "${PROJECT_SOURCE_DIR}/include")
set_property(TARGET logconduit PROPERTY _LOGCONDUIT_TEMPLATE_DIR
  "${PROJECT_SOURCE_DIR}/template")
set_property(TARGET logconduit PROPERTY _LOGCONDUIT_SOURCE_DIR "${PROJECT_SOURCE_DIR}")
include(cmake/logconduit-helpers.cmake)
