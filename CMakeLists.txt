if(WIN32)
	# Required for MSVC_RUNTIME_LIBRARY
	cmake_minimum_required(VERSION 3.15)
	cmake_policy(SET CMP0091 NEW)
else()
	cmake_minimum_required(VERSION 3.19)
endif()

project(gmod-html LANGUAGES CXX)

# We can only do release builds
set(CMAKE_CONFIGURATION_TYPES Release Debug)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Match GMod and our prebuilt libraries
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out)
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/dist CACHE PATH "..." FORCE)
endif()

if(WIN32)
	if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "4")
		set(GAME_BIN_DIR .)
	else()
		set(GAME_BIN_DIR win64)
	endif()
elseif(UNIX AND NOT APPLE)
	set(GAME_BIN_DIR linux64)
else()
	message(FATAL_ERROR "No GAME_DIR definition")
endif()


# ImGui
add_subdirectory(thirdparty/imgui-1.74)

# glad
add_subdirectory(thirdparty/glad)

# GLFW
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(thirdparty/glfw-3.3.2)

# Chromium Project
if(WIN32)
	if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "4")
		set(CEF_PATH ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/cef3/cef_binary_96.0.16+g89c902b+chromium-96.0.4664.55_windows32)
	else()
		set(CEF_PATH ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/cef3/cef_binary_96.0.16+g89c902b+chromium-96.0.4664.55_windows64)
	endif()
elseif(UNIX AND NOT APPLE)
	set(CEF_PATH ${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/cef3/cef_binary_96.0.16+g89c902b+chromium-96.0.4664.55_linux64)
	add_definitions(-DPOSIX -DLINUX)
else()
	message(FATAL_ERROR "No CEF_PATH")
endif()

add_subdirectory(${CEF_PATH} EXCLUDE_FROM_ALL)

# Chromium Import Lib
add_library(libcef_imp SHARED IMPORTED)

if(WIN32)
	set_target_properties(libcef_imp PROPERTIES IMPORTED_LOCATION ${CEF_PATH}/Release/libcef.dll)
	set_target_properties(libcef_imp PROPERTIES IMPORTED_IMPLIB ${CEF_PATH}/Release/libcef.lib)
elseif(UNIX AND NOT APPLE)
	set_target_properties(libcef_imp PROPERTIES IMPORTED_LOCATION ${CEF_PATH}/Release/libcef.so)
else()
	message(FATAL_ERROR "No libcef_imp import library set")
endif()

# Chromium Sandbox (Not a library on Linux)
if(WIN32 OR APPLE)
	add_library(cef_sandbox STATIC IMPORTED)

	if(WIN32)
		set_target_properties(cef_sandbox PROPERTIES IMPORTED_LOCATION ${CEF_PATH}/Release/cef_sandbox.lib)
	else()
		message(FATAL_ERROR "No cef_sandbox library set")
	endif()
endif()

# Our projects
add_subdirectory(html)
add_subdirectory(html_stub)
add_subdirectory(html_chromium)

if(WIN32)
	add_subdirectory(example_host)
	add_subdirectory(gmod_launcher)
endif()

if(UNIX)
	add_subdirectory(html_chromium/chromium_process)
endif()
