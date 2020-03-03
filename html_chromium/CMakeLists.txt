if(WIN32)
	set(BINARIES
		${CEF_PATH}/Release/chrome_elf.dll
		${CEF_PATH}/Release/d3dcompiler_47.dll
		${CEF_PATH}/Release/libcef.dll
		${CEF_PATH}/Release/libEGL.dll
		${CEF_PATH}/Release/libGLESv2.dll
		${CEF_PATH}/Release/snapshot_blob.bin
		${CEF_PATH}/Release/v8_context_snapshot.bin
		${CEF_PATH}/Resources/icudtl.dat)
elseif(UNIX AND NOT APPLE)
	set(BINARIES
		${CEF_PATH}/Release/chrome-sandbox
		${CEF_PATH}/Release/libcef.so
		${CEF_PATH}/Release/libEGL.so
		${CEF_PATH}/Release/libGLESv2.so
		${CEF_PATH}/Release/snapshot_blob.bin
		${CEF_PATH}/Release/v8_context_snapshot.bin
		${CEF_PATH}/Resources/icudtl.dat)
else()
	message(FATAL_ERROR "No IMPORTED_LOCATION for libcef")
endif()

set(RESOURCES
	${CEF_PATH}/Resources/cef.pak
	${CEF_PATH}/Resources/cef_100_percent.pak
	${CEF_PATH}/Resources/cef_200_percent.pak
	${CEF_PATH}/Resources/cef_extensions.pak
	${CEF_PATH}/Resources/devtools_resources.pak)

install(FILES ${BINARIES} DESTINATION ${GAME_BIN_DIR})
install(FILES ${RESOURCES} DESTINATION ${CMAKE_BINARY_DIR}/bin/chromium)
install(DIRECTORY ${CEF_PATH}/Resources/locales DESTINATION ${CMAKE_BINARY_DIR}/bin/chromium)

# Actual lib
set(SOURCES
	cef_end.h
	cef_start.h
	ChromiumBrowser.cpp
	ChromiumBrowser.h
	ChromiumClient.cpp
	ChromiumClient.h
	ChromiumSystem.cpp
	ChromiumSystem.h
	HtmlResourceHandler.cpp
	HtmlResourceHandler.h
	ImageData.h
	JSObjects.cpp
	JSObjects.h
	MessageQueue.h
	ResourceHandler.cpp
	ResourceHandler.h)

# Lame
include_directories(${CEF_PATH})

add_library(html_chromium SHARED ${SOURCES})
target_link_libraries(html_chromium html libcef_imp libcef_dll_wrapper)
SET_TARGET_PROPERTIES(html_chromium PROPERTIES PREFIX "")
