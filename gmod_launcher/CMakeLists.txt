set(TARGET gmod_launcher)

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
	set(SOURCES
		Linux.cpp)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--export-dynamic") # used for exposing the "has_namespace_support" variable for dlsym
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
	set(SOURCES
		MacOS.cpp)
endif()

add_executable(${TARGET} ${SOURCES})
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
target_link_libraries(${TARGET} ${CMAKE_DL_LIBS})

if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
	install(TARGETS ${TARGET} DESTINATION "${INSTALL_OUT_DIR}/GarrysMod/bin/${CEF_PLATFORM}")
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
	install(TARGETS ${TARGET} DESTINATION "${INSTALL_OUT_DIR}/GarrysMod_Signed.app/Contents/MacOS")
endif()

set_target_properties(${TARGET} PROPERTIES INSTALL_RPATH "$ORIGIN")
set_target_properties(${TARGET} PROPERTIES BUILD_WITH_INSTALL_RPATH TRUE)
set_target_properties(${TARGET} PROPERTIES OUTPUT_NAME "gmod")

# Always generate debug symbols
if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux" OR "${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
	set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -g")
endif()
