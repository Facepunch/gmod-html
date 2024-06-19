
cmake_minimum_required(VERSION 3.19)
set(CMAKE_CONFIGURATION_TYPES Debug Release)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15")

project(gmod_html)

set_property(GLOBAL PROPERTY OS_FOLDERS ON)

set(CEF_VERSION "124.3.5+gff7dcd8+chromium-124.0.6367.119")

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
  if("${PROJECT_ARCH}" STREQUAL "arm64")
    set(CEF_PLATFORM "macosarm64")
  else()
    set(CEF_PLATFORM "macosx64")
  endif()
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
  if(CMAKE_SIZEOF_VOID_P MATCHES 8)
    set(CEF_PLATFORM "linux64")
  else()
    set(CEF_PLATFORM "linux32")
  endif()
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
  if(CMAKE_SIZEOF_VOID_P MATCHES 8)
    set(CEF_PLATFORM "windows64")
  else()
    set(CEF_PLATFORM "windows32")
  endif()
endif()

set(CEF_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/cef3/cef_binary_${CEF_VERSION}_${CEF_PLATFORM}")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CEF_ROOT}/cmake")

find_package(CEF REQUIRED)
add_subdirectory(thirdparty/glad EXCLUDE_FROM_ALL)
add_subdirectory(thirdparty/imgui-1.74 EXCLUDE_FROM_ALL)

set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(thirdparty/glfw-3.3.2 EXCLUDE_FROM_ALL)

if(GEN_NINJA OR GEN_MAKEFILES)
  set(CEF_TARGET_OUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/out/${CMAKE_BUILD_TYPE}/${CEF_PLATFORM}")
  set(INSTALL_OUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/dist/${CEF_PLATFORM}-${CMAKE_BUILD_TYPE}")
else()
  set(CEF_TARGET_OUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/out/$<CONFIGURATION>/${CEF_PLATFORM}")
  set(INSTALL_OUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/dist/${CEF_PLATFORM}-$<CONFIGURATION>")
endif()
set(CEF_TARGET_OUT_DIR ${CEF_TARGET_OUT_DIR}/bin/${CEF_PLATFORM})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CEF_TARGET_OUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CEF_TARGET_OUT_DIR})

add_subdirectory(${CEF_LIBCEF_DLL_WRAPPER_PATH} libcef_dll_wrapper EXCLUDE_FROM_ALL)

add_subdirectory(${CEF_ROOT}/tests/cefclient EXCLUDE_FROM_ALL)
add_subdirectory(${CEF_ROOT}/tests/cefsimple EXCLUDE_FROM_ALL)
add_subdirectory(${CEF_ROOT}/tests/gtest EXCLUDE_FROM_ALL)
add_subdirectory(${CEF_ROOT}/tests/ceftests EXCLUDE_FROM_ALL)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

add_subdirectory(html)
add_subdirectory(html_stub EXCLUDE_FROM_ALL)
add_subdirectory(html_chromium)
add_subdirectory(chromium_process)
add_subdirectory(example_host EXCLUDE_FROM_ALL)
if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
  add_subdirectory(gmod)
endif()

PRINT_CEF_CONFIG()
