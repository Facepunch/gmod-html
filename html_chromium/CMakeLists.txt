
set(TARGET html_chromium)
set(SOURCES 
	cef_end.h
	cef_start.h
	chromium/dom_code_data.inc
	chromium/dom_code.h
	chromium/keycode_converter.cc
	chromium/keycode_converter.h
	DomCodeConverter.h
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

if(OS_LINUX OR OS_WINDOWS)
	ADD_LOGICAL_TARGET("libcef_lib" "${CEF_LIB_DEBUG}" "${CEF_LIB_RELEASE}")
endif()

add_library(${TARGET} SHARED ${SOURCES})
if (DUMP_SYMS)
	if(OS_LINUX OR OS_MAC)
		add_custom_command(
			TARGET ${TARGET}
			POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E echo \"Dumping symbols for ${TARGET}\"
			COMMAND "${DUMP_SYMS}" -s "${CMAKE_SOURCE_DIR}/syms" "$<TARGET_FILE:${TARGET}>"
			COMMAND "${STRIP}" $STRIP_ARGS "$<TARGET_FILE:${TARGET}>"
			VERBATIM
		)
	else()
		add_custom_command(
			TARGET ${TARGET}
			POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E echo \"Dumping symbols for ${TARGET}\"
			COMMAND "${DUMP_SYMS}" -s "${CMAKE_SOURCE_DIR}/syms" "$<TARGET_FILE:${TARGET}>"
			VERBATIM
		)
	endif()
endif()
SET_LIBRARY_TARGET_PROPERTIES(${TARGET})
add_dependencies(${TARGET} html libcef_dll_wrapper)

if(OS_MAC)
	target_link_libraries(${TARGET} html libcef_dll_wrapper ${CEF_STANDARD_LIBS})
else()
	target_link_libraries(${TARGET} html libcef_lib libcef_dll_wrapper ${CEF_STANDARD_LIBS})
endif()

if(OS_WINDOWS AND USE_SANDBOX)
	ADD_LOGICAL_TARGET("cef_sandbox_lib" "${CEF_SANDBOX_LIB_DEBUG}" "${CEF_SANDBOX_LIB_RELEASE}")
	target_link_libraries(${TARGET} cef_sandbox_lib ${CEF_SANDBOX_STANDARD_LIBS})
endif()

set_target_properties(${TARGET} PROPERTIES PREFIX "")
if(OS_LINUX)
	set_target_properties(${TARGET} PROPERTIES OUTPUT_NAME "html_chromium_client")
endif()

if(OS_WINDOWS)
	if(CMAKE_SIZEOF_VOID_P MATCHES 8)
		install(TARGETS ${TARGET} DESTINATION "${INSTALL_OUT_DIR}/GarrysMod/bin/win64")
	else()
		install(TARGETS ${TARGET} DESTINATION "${INSTALL_OUT_DIR}/GarrysMod/bin")
	endif()
elseif(OS_LINUX)
	install(TARGETS ${TARGET} DESTINATION "${INSTALL_OUT_DIR}/GarrysMod/bin/${CEF_PLATFORM}")
elseif(OS_MAC)
	install(TARGETS ${TARGET} DESTINATION "${INSTALL_OUT_DIR}/GarrysMod_Signed.app/Contents/MacOS")
endif()

# Always generate debug symbols
if(OS_LINUX OR OS_MAC)
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -g")
elseif(OS_WINDOWS)
	target_link_options(${TARGET} PUBLIC /DEBUG)
endif()
