set(TARGET gmod_32bit_redirector)

set(SOURCES
	Windows.cpp)
set(RESOURCES
	../chromium_process/resources/win/gmod_icon.rc)

#APPEND_PLATFORM_SOURCES(SOURCES)
#APPEND_PLATFORM_SOURCES(RESOURCES)

add_executable(${TARGET} WIN32 ${SOURCES} ${RESOURCES})
if (DUMP_SYMS)
	add_custom_command(
		TARGET ${TARGET}
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E echo \"Dumping symbols for ${TARGET}\"
		COMMAND "${DUMP_SYMS}" -s "${CMAKE_SOURCE_DIR}/syms" "$<TARGET_FILE:${TARGET}>"
		VERBATIM
	)
endif()

SET_EXECUTABLE_TARGET_PROPERTIES(${TARGET})

install(TARGETS ${TARGET} DESTINATION "${INSTALL_OUT_DIR}/GarrysMod")

target_link_libraries(${TARGET}
	shlwapi)

set_target_properties(${TARGET} PROPERTIES INSTALL_RPATH "$ORIGIN")
set_target_properties(${TARGET} PROPERTIES BUILD_WITH_INSTALL_RPATH TRUE)
#set_target_properties(${TARGET} PROPERTIES OUTPUT_NAME "gmod")

# Always generate debug symbols
target_link_options(${TARGET} PUBLIC /DEBUG)
