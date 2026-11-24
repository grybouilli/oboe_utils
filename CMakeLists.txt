cmake_minimum_required(VERSION 3.11...3.18)

project(oboe_utils
  VERSION 1.0.0
  DESCRIPTION "C++ library providing helpers for use of Oboe"
  LANGUAGES CXX
)

option(BUILD_DOCS "Build and Install Documents (Requires Doxygen)" OFF)
option(BUILD_EXAMPLE_APP "Build example passthrough app" OFF)
option(${PROJECT_NAME}_BUILD_TESTING "Build and run tests" OFF)

## Make sure that custom modules like FindXYZ are found
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

include(FetchContent)
include(GNUInstallDirs)
include(cmake/FetchOboe.cmake)

## Define Target
## =============

add_library(${PROJECT_NAME} SHARED src/stream_manipulation.cpp)
target_include_directories(${PROJECT_NAME}
  PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_compile_features(${PROJECT_NAME} INTERFACE cxx_std_20)

target_include_directories(${PROJECT_NAME}
  PUBLIC oboe)
target_link_libraries(${PROJECT_NAME}
  PUBLIC oboe
)

if(PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_DOCS)
  find_package(Doxygen REQUIRED dot)
  if(Doxygen_FOUND)
    add_subdirectory(docs)
  else()
    message(STATUS "Doxygen not found, not building docs")
  endif()
endif()

if(BUILD_EXAMPLE_APP)
  set(PASSTHROUGH_APP_BIN passthrough_app)
  add_executable(${PASSTHROUGH_APP_BIN} src/example_app.cpp)
  target_link_libraries(${PASSTHROUGH_APP_BIN} PRIVATE ${PROJECT_NAME})
endif()

## Testing
## =======

if(PROJECT_NAME STREQUAL PROJECT_NAME)
  include(CTest)
endif()

## flag RMOLIB_BUILD_TESTING for enable testing anyway
if((PROJECT_NAME STREQUAL PROJECT_NAME OR ${PROJECT_NAME}_BUILD_TESTING) AND BUILD_TESTING)
  add_subdirectory(test)
endif()
