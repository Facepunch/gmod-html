cmake_minimum_required(VERSION 3.15)

# Required for MSVC_RUNTIME_LIBRARY
if(WIN32)
	cmake_policy(SET CMP0091 NEW)
endif()

project(gmod_html)

# We can only do release builds
set(CMAKE_CONFIGURATION_TYPES Release Debug)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set_property(GLOBAL PROPERTY OS_FOLDERS ON)

set(CEF_VERSION "86.0.24+g85e79d4+chromium-86.0.4240.198")

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

# glad
add_subdirectory(thirdparty/glad EXCLUDE_FROM_ALL)

# ImGui
add_subdirectory(thirdparty/imgui-1.74 EXCLUDE_FROM_ALL)

# GLFW
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(thirdparty/glfw-3.3.2 EXCLUDE_FROM_ALL)

# Chromium Project
set(CEF_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/cef3/cef_binary_${CEF_VERSION}_${CEF_PLATFORM}")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CEF_ROOT}/cmake")

# OS_WINDOWS, OS_LINUX, and OS_MAC are defined in cef_variables.cmake
find_package(CEF REQUIRED)

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

# Match GMod and our prebuilt libraries
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# Our projects
add_subdirectory(html)
add_subdirectory(html_stub EXCLUDE_FROM_ALL)
add_subdirectory(html_chromium)

if(OS_LINUX OR OS_MAC)
	add_subdirectory(html_chromium/chromium_process)
endif()

# Disabled for now
#add_subdirectory(example_host EXCLUDE_FROM_ALL)

PRINT_CEF_CONFIG()
