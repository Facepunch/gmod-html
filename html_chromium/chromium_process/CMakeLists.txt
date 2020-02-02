set(SOURCES
	ChromiumApp.cpp
	ChromiumApp.h)

if(UNIX AND NOT APPLE)
	set(SOURCES ${SOURCES} Linux.cpp)
else()
	message(FATAL_ERROR "No ChromiumApp Main implementation")
endif()

# Lame
include_directories(${CEF_PATH})

add_executable(chromium_process ${SOURCES})
target_link_libraries(chromium_process libcef_imp libcef_dll_wrapper)
install(TARGETS chromium_process RUNTIME DESTINATION ${GAME_BIN_DIR})

