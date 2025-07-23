
set(TARGET example_host)
set(SOURCES
	Main.cpp
	Window.cpp
	Window.h
	HtmlSystemLoader.cpp
	HtmlSystemLoader.h
	HtmlResourceHandler.cpp
	HtmlResourceHandler.h
	HtmlPanel.cpp
	HtmlPanel.h)

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
SET_EXECUTABLE_TARGET_PROPERTIES(${TARGET})
add_dependencies(${TARGET} html_chromium html chromium_process glfw glad imgui)
target_compile_definitions(${TARGET} PRIVATE IMGUI_IMPL_OPENGL_LOADER_GLAD)
target_link_libraries(${TARGET} glfw glad imgui html)

if(OS_LINUX)
	set_target_properties(${TARGET} PROPERTIES INSTALL_RPATH "$ORIGIN")
	set_target_properties(${TARGET} PROPERTIES BUILD_WITH_INSTALL_RPATH TRUE)
endif()
