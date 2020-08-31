cmake_minimum_required(VERSION 3.10)
project(scsdk C CXX)

enable_testing()

set(CMAKE_CXX_STANDARD 17)


###############################################################################
## Global Includes ############################################################
###############################################################################

## Include the C++ root project
include_directories("${PROJECT_SOURCE_DIR}/scsdk")

## PRIVATE External Dependencies
set(LIBSC_DEPS_ROOT "../../deps/")
include_directories("${LIBSC_DEPS_ROOT}/happly")
include_directories("${LIBSC_DEPS_ROOT}/json/include")
include_directories("${LIBSC_DEPS_ROOT}/tinygltf")
include_directories("${LIBSC_DEPS_ROOT}/stb")
include_directories("${LIBSC_DEPS_ROOT}/SparseICP")
include_directories("${LIBSC_DEPS_ROOT}/nanoflann/include")
include_directories("${LIBSC_DEPS_ROOT}/libigl/include")

## PUBLIC External Dependencies
set(LIBSC_PODS_ROOT "../../Pods/")
include_directories("${LIBSC_PODS_ROOT}/Protobuf-C++/src")
include_directories("${LIBSC_PODS_ROOT}/ProtobagCocoa/c++/protobag")
include_directories("${LIBSC_PODS_ROOT}/FMTCocoa/fmt_root/include")
include_directories("${LIBSC_PODS_ROOT}/LibArchiveCocoa/libarchive/libarchive")

include_directories("${LIBSC_PODS_ROOT}/EigenCPPCocoa/")

if(JS_ONLY)
  # Use emscripten-tuned config.h
  include_directories("${PROJECT_SOURCE_DIR}/emscripten_libarchive_support")
else()
  # Use platform config.h
  if(APPLE)
    include_directories("${LIBSC_PODS_ROOT}/LibArchiveCocoa/config/mac")
  else()
    include_directories("${LIBSC_PODS_ROOT}/LibArchiveCocoa/config/ubuntu")
  endif()
  include_directories("${LIBSC_PODS_ROOT}/LibArchiveCocoa/")
endif()

## Compile third party sources into the library
file(
  GLOB_RECURSE libsc_pods_srcs
  
  ${LIBSC_PODS_ROOT}/Protobuf-C++/src/*.cc
  
  ${LIBSC_PODS_ROOT}/ProtobagCocoa/c++/protobag/*.cc
  ${LIBSC_PODS_ROOT}/ProtobagCocoa/c++/protobag/*.cpp

  ${LIBSC_PODS_ROOT}/LibArchiveCocoa/libarchive/*.c)


###############################################################################
## Global Flags ###############################################################
###############################################################################

# FMT: Use in header-only mode
add_definitions(-DFMT_HEADER_ONLY)
  # NB: https://github.com/fmtlib/fmt/issues/524

# LibArchive
# * use our custom config.h
# * for mac, link libiconv and gzip libs
add_definitions(-DHAVE_CONFIG_H=1)

if(NOT JS_ONLY)
  if (NOT APPLE)
    link_libraries(uuid c++fs)
  endif()

  link_libraries(z bz2)
endif()

# Protobuf: needs pthread
add_definitions(-DHAVE_PTHREAD=1)

# For scsdk-included guid.cpp
add_definitions(-DGUID_LIBUUID)
if(NOT APPLE AND NOT JS_ONLY)
  link_libraries(uuid)
endif()

set(libsc_common_flags "-Wall -std=c++17 -stdlib=libc++")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${libsc_common_flags}")
if(UNIX OR APPLE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
endif()
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} ${libsc_common_flags} -g3")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} ${libsc_common_flags} -O3")




###############################################################################
## Library scsdk ##################################################
###############################################################################

file(
  GLOB_RECURSE libsc_srcs
  scsdk/*.h
  scsdk/*.hpp
  scsdk/*.cpp
  scsdk/*.cc)

# Compile third party sources with our own-- need this for cross-platform
set(libsc_srcs ${libsc_srcs} ${libsc_pods_srcs})

set(
  libsc_dep_libs
  pthread)


# Include C++17 std::filesystem library
if(NOT APPLE)
  set(libsc_dep_libs ${libsc_dep_libs} c++fs)
endif()

if(NOT JS_ONLY)
add_library(scsdk SHARED ${libsc_srcs})
add_library(scsdkStatic STATIC ${libsc_srcs})
set_target_properties(
  scsdkStatic PROPERTIES OUTPUT_NAME scsdk)

target_link_libraries(
  scsdk
  PRIVATE
  ${libsc_dep_libs})
endif() # NOT JS_ONLY


###############################################################################
## GTest-based Executable scsdk_test ##############################
###############################################################################

if(NOT JS_ONLY)

# Set Up GTest
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.10.0)

FetchContent_GetProperties(googletest)
if(NOT googletest_POPULATED)
  FetchContent_Populate(googletest)
  add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
endif()
set(BUILD_GTEST ON CACHE BOOL "" FORCE)

file(
  GLOB_RECURSE libsc_test_srcs
  scsdk_test/*.h
  scsdk_test/*.hpp
  scsdk_test/*.cpp
  scsdk_test/*.cc)

add_executable(scsdk_test ${libsc_test_srcs})

# Tell `scsdk_test` where to find test fixtures by default
# Users can override at runtime usng the `SC_TEST_FIXTURES_DIR` env var.
set(
  SC_TEST_DEFAULT_FIXTURES_DIR
  "${PROJECT_SOURCE_DIR}/test_fixture_data/")
target_compile_definitions(
  scsdk_test
  PRIVATE
  -DSC_TEST_DEFAULT_FIXTURES_DIR="${SC_TEST_DEFAULT_FIXTURES_DIR}" )

target_include_directories(
  scsdk_test
  PRIVATE
  ${PROJECT_SOURCE_DIR}/scsdk_test
  ${gtest_SOURCE_DIR}/include
  ${gtest_SOURCE_DIR})

target_link_libraries(
  scsdk_test
  PRIVATE
  scsdkStatic
  gtest
  gtest_main)

add_test(
  NAME test 
  COMMAND bash -c "$<TARGET_FILE:scsdk_test>")
set_tests_properties(test PROPERTIES DEPENDS scsdk_test)

endif() # End NOT JS_ONLY



###############################################################################
## scsdk Programs #################################################
###############################################################################

if(NOT JS_ONLY)


endif() # End NOT JS_ONLY



###############################################################################
## Python Library standard_cyborg_sdk #########################################
###############################################################################

if(PYBIND11_ONLY)

# Tell pybind to use python3
if(NOT PYTHON_EXECUTABLE)
  execute_process(COMMAND which python3 OUTPUT_VARIABLE PYTHON_EXECUTABLE)
endif()

add_subdirectory(scsdkPython/pybind11)
pybind11_add_module(
  scsdk_native
  ${libsc_srcs}
  scsdkPython/scsdk_native.cpp)
target_compile_definitions(
  scsdk_native
  PRIVATE
  -DPYBIND11_ONLY)
target_link_libraries(
  scsdk_native
  PUBLIC
  ${libsc_dep_libs})

endif() # End PYBIND11_ONLY



# ###############################################################################
# ## Emscripten Library scsdk.js ####################################
# ###############################################################################

if(JS_ONLY)
add_executable(
  scsdk_js
  ${libsc_srcs}
  scsdkJS/scsdk_JS.cpp)
set_target_properties(
  scsdk_js
  PROPERTIES 
    OUTPUT_NAME "scsdk"
    SUFFIX ".js")


get_property(
  libsc_js_includes
  DIRECTORY
  ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)

target_compile_options(
  scsdk_js
  PRIVATE
  -O2
  --llvm-lto 1
  -DEMBIND_ONLY
  --bind
  "SHELL:-s WASM=1"
  "SHELL:-s ALLOW_MEMORY_GROWTH=1"
  "SHELL:-s MODULARIZE=1"
  "SHELL:-s FORCE_FILESYSTEM=1"  
  "SHELL:-s USE_ZLIB=1"         # For libarchive / protobag
  "SHELL:-s USE_BZIP2=1"        # For libarchive / protobag
  )

set_target_properties(
  scsdk_js
  PROPERTIES LINK_FLAGS 
  ${LINK_FLAGS} "--bind -s ALLOW_MEMORY_GROWTH=1 --llvm-lto 1 -s WASM=1 -s MODULARIZE=1 -s FORCE_FILESYSTEM=1 --pre-js ${PROJECT_SOURCE_DIR}/scsdkJS/pre-js.js")

endif() # END JS_ONLY

