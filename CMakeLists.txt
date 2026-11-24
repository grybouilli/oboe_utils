cmake_minimum_required(VERSION 3.11...3.18)

project(oboe_utils
  VERSION 1.0.0
  DESCRIPTION "C++ library providing helpers for use of Oboe"
  LANGUAGES CXX
)

option(BUILD_DOCS "Build and Install Documents (Requires Doxygen)" OFF)
option(${CMAKE_PROJECT_NAME}_BUILD_TESTING "Build and run tests" OFF)

## Make sure that custom modules like FindXYZ are found
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

include(FetchContent)
include(GNUInstallDirs)
include(cmake/FetchOboe.cmake)

## Define Target
## =============

add_library(${CMAKE_PROJECT_NAME} SHARED src/stream_manipulation.cpp)
target_include_directories(${CMAKE_PROJECT_NAME}
  PRIVATE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/include>
)
target_compile_features(${CMAKE_PROJECT_NAME} INTERFACE cxx_std_14)

target_link_libraries(${CMAKE_PROJECT_NAME}
  PRIVATE oboe
)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_DOCS)
  find_package(Doxygen REQUIRED dot)
  if(Doxygen_FOUND)
    add_subdirectory(docs)
  else()
    message(STATUS "Doxygen not found, not building docs")
  endif()
endif()

## Testing
## =======

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  include(CTest)
endif()

## flag RMOLIB_BUILD_TESTING for enable testing anyway
if((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME OR ${CMAKE_PROJECT_NAME}_BUILD_TESTING) AND BUILD_TESTING)
  add_subdirectory(test)
endif()
