
cmake_minimum_required(VERSION 3.19)
set(CMAKE_CONFIGURATION_TYPES Debug Release)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(gmod_html)

set_property(GLOBAL PROPERTY OS_FOLDERS ON)

set(CEF_VERSION "98.2.0+g78c653a+chromium-98.0.4758.102")

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
  set(CEF_TARGET_OUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/out/${CMAKE_BUILD_TYPE}")
else()
  set(CEF_TARGET_OUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/out/$<CONFIGURATION>")
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

add_subdirectory(html)
# NOTE: This can probably be removed. It would only need rebuilding if the html interface changes (and this is unlikely).
add_subdirectory(html_stub)
add_subdirectory(html_chromium)

if(OS_LINUX OR OS_MAC)
  add_subdirectory(chromium_process)
elseif(OS_WIN)
  add_subdirectory(gmod_launcher)
endif()

add_subdirectory(example_host EXCLUDE_FROM_ALL)

PRINT_CEF_CONFIG()