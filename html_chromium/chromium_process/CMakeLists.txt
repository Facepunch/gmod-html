set(SOURCES
	ChromiumApp.cpp
	ChromiumApp.h)

if(WIN32)
	set(SOURCES ${SOURCES} Windows.cpp)
elseif(UNIX AND NOT APPLE)
	set(SOURCES ${SOURCES} Linux.cpp)
else()
	set(SOURCES ${SOURCES} macOS.cpp)
endif()

# Lame
include_directories(${CEF_PATH})

add_executable(chromium_process ${SOURCES})
target_link_libraries(chromium_process libcef_imp libcef_dll_wrapper)
install(TARGETS chromium_process RUNTIME DESTINATION ${GAME_BIN_DIR})
