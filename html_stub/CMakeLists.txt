set(TARGET html_stub)
set(SOURCES
	StubClient.cpp
	StubClient.h
	StubSystem.cpp
	StubSystem.h)

add_library(${TARGET} SHARED ${SOURCES})
add_dependencies(${TARGET} html)
target_link_libraries(${TARGET} html)

set_target_properties(${TARGET} PROPERTIES PREFIX "")
if(OS_LINUX)
	set_target_properties(${TARGET} PROPERTIES OUTPUT_NAME "html_stub_client")
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
