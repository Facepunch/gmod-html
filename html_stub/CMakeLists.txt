set(SOURCES
	StubClient.cpp
	StubClient.h
	StubSystem.cpp
	StubSystem.h)

add_library(html_stub SHARED ${SOURCES})
if (DUMP_SYMS)
	if(OS_LINUX OR OS_MAC)
		add_custom_command(
			TARGET html_stub
			POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E echo \"Dumping symbols for html_stub\"
			COMMAND "${DUMP_SYMS}" -s "${CMAKE_SOURCE_DIR}/syms" "$<TARGET_FILE:html_stub>"
			COMMAND "${STRIP}" $STRIP_ARGS "$<TARGET_FILE:html_stub>"
			VERBATIM
		)
	else()
		add_custom_command(
			TARGET html_stub
			POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E echo \"Dumping symbols for html_stub\"
			COMMAND "${DUMP_SYMS}" -s "${CMAKE_SOURCE_DIR}/syms" "$<TARGET_FILE:html_stub>"
			VERBATIM
		)
	endif()
endif()

add_dependencies(html_stub html)
target_link_libraries(html_stub html)

set_target_properties(html_stub PROPERTIES PREFIX "")
if(OS_LINUX)
	set_target_properties(html_stub PROPERTIES OUTPUT_NAME "html_stub_client")
endif()

if(OS_WINDOWS)
	if(CMAKE_SIZEOF_VOID_P MATCHES 8)
		install(TARGETS html_stub DESTINATION "${INSTALL_OUT_DIR}/GarrysMod/bin/win64")
	else()
		install(TARGETS html_stub DESTINATION "${INSTALL_OUT_DIR}/GarrysMod/bin")
	endif()
elseif(OS_LINUX)
	install(TARGETS html_stub DESTINATION "${INSTALL_OUT_DIR}/GarrysMod/bin/${CEF_PLATFORM}")
elseif(OS_MAC)
	install(TARGETS html_stub DESTINATION "${INSTALL_OUT_DIR}/GarrysMod_Signed.app/Contents/MacOS")
endif()
