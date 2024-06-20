
set(TARGET chromium_process)
set(SOURCES
	ChromiumApp.cpp
	ChromiumApp.h)
set(RESOURCES )

set(SOURCES_LINUX Linux.cpp)
set(SOURCES_MAC macOS.cpp)

set(SOURCES_WINDOWS Windows.cpp)
set(RESOURCES_WINDOWS resources/win/gmod_icon.rc)

APPEND_PLATFORM_SOURCES(SOURCES)
APPEND_PLATFORM_SOURCES(RESOURCES)

if(OS_LINUX OR OS_WINDOWS)
	ADD_LOGICAL_TARGET("libcef_lib" "${CEF_LIB_DEBUG}" "${CEF_LIB_RELEASE}")

	if(OS_LINUX)
		add_executable(${TARGET} ${SOURCES})
	else()
		add_executable(${TARGET} WIN32 ${SOURCES} ${RESOURCES})
	endif()
	SET_EXECUTABLE_TARGET_PROPERTIES(${TARGET})
	add_dependencies(${TARGET} libcef_dll_wrapper)

	target_link_libraries(${TARGET} libcef_lib libcef_dll_wrapper ${CEF_STANDARD_LIBS})

	if(OS_LINUX)
		install(TARGETS ${TARGET} DESTINATION "${INSTALL_OUT_DIR}/GarrysMod/bin/${CEF_PLATFORM}")
		set_target_properties(${TARGET} PROPERTIES INSTALL_RPATH "$ORIGIN")
		set_target_properties(${TARGET} PROPERTIES BUILD_WITH_INSTALL_RPATH TRUE)
	else()
		if(CMAKE_SIZEOF_VOID_P MATCHES 8)
			install(TARGETS ${TARGET} DESTINATION "${INSTALL_OUT_DIR}/GarrysMod/bin/win64")
		else()
			install(TARGETS ${TARGET} DESTINATION "${INSTALL_OUT_DIR}/GarrysMod/bin")
		endif()
		
		set_target_properties(${TARGET} PROPERTIES OUTPUT_NAME "gmod")
		target_link_libraries(${TARGET}
			shlwapi
			winmm
			wsock32
			WS2_32
			comctl32
			rpcrt4
			version
			DbgHelp
			Psapi
			wbemuuid
			OleAut32
			SetupAPI
			Propsys
			Cfgmgr32
			PowrProf
			Delayimp.lib)

		if(USE_SANDBOX)
			ADD_LOGICAL_TARGET("cef_sandbox_lib" "${CEF_SANDBOX_LIB_DEBUG}" "${CEF_SANDBOX_LIB_RELEASE}")
			target_link_libraries(${TARGET} cef_sandbox_lib ${CEF_SANDBOX_STANDARD_LIBS})
		endif()
	endif()

	# FIXME: These should be moved to their own target
	LIST(REMOVE_ITEM CEF_RESOURCE_FILES "icudtl.dat")
	if(OS_LINUX)
		COPY_FILES(${TARGET} "icudtl.dat" "${CEF_RESOURCE_DIR}" "${INSTALL_OUT_DIR}/GarrysMod/bin/${CEF_PLATFORM}")
		COPY_FILES(${TARGET} "${CEF_BINARY_FILES}" "${CEF_BINARY_DIR}" "${INSTALL_OUT_DIR}/GarrysMod/bin/${CEF_PLATFORM}")
		COPY_FILES(${TARGET} "${CEF_RESOURCE_FILES}" "${CEF_RESOURCE_DIR}" "${INSTALL_OUT_DIR}/GarrysMod/bin/linux32/chromium")
	else()
		if(CMAKE_SIZEOF_VOID_P MATCHES 8)
			COPY_FILES(${TARGET} "icudtl.dat" "${CEF_RESOURCE_DIR}" "${INSTALL_OUT_DIR}/GarrysMod/bin/win64")
			COPY_FILES(${TARGET} "${CEF_BINARY_FILES}" "${CEF_BINARY_DIR}" "${INSTALL_OUT_DIR}/GarrysMod/bin/win64")
		else()
			COPY_FILES(${TARGET} "icudtl.dat" "${CEF_RESOURCE_DIR}" "${INSTALL_OUT_DIR}/GarrysMod/bin")
			COPY_FILES(${TARGET} "${CEF_BINARY_FILES}" "${CEF_BINARY_DIR}" "${INSTALL_OUT_DIR}/GarrysMod/bin")
		endif()
		COPY_FILES(${TARGET} "${CEF_RESOURCE_FILES}" "${CEF_RESOURCE_DIR}" "${INSTALL_OUT_DIR}/GarrysMod/bin/chromium")
	endif()
else()
	set(HELPER_TARGET "gmod_Helper")
	set(HELPER_OUTPUT_NAME "gmod Helper")
	add_custom_target(${TARGET} ALL)

	if(USE_SANDBOX)
		ADD_LOGICAL_TARGET("cef_sandbox_lib" "${CEF_SANDBOX_LIB_DEBUG}" "${CEF_SANDBOX_LIB_RELEASE}")
	endif()

	add_custom_command(
		TARGET ${TARGET}
		POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory
						"${CEF_BINARY_DIR}/Chromium Embedded Framework.framework"
						"${INSTALL_OUT_DIR}/GarrysMod_Signed.app/Contents/Frameworks/Chromium Embedded Framework.framework"
		VERBATIM)

	foreach(_suffix_list ${CEF_HELPER_APP_SUFFIXES})
		string(REPLACE ":" ";" _suffix_list ${_suffix_list})
		list(GET _suffix_list 0 _name_suffix)
		list(GET _suffix_list 1 _target_suffix)
		list(GET _suffix_list 2 _plist_suffix)

		set(_helper_target "${HELPER_TARGET}${_target_suffix}")
		set(_helper_output_name "${HELPER_OUTPUT_NAME}${_name_suffix}")

		set(_helper_info_plist "${CMAKE_CURRENT_BINARY_DIR}/helper-Info${_target_suffix}.plist")
		file(READ "${CMAKE_CURRENT_SOURCE_DIR}/resources/mac/helper-Info.plist" _plist_contents)
		string(REPLACE "\${EXECUTABLE_NAME}" "${_helper_output_name}" _plist_contents ${_plist_contents})
		string(REPLACE "\${PRODUCT_NAME}" "${_helper_output_name}" _plist_contents ${_plist_contents})
		string(REPLACE "\${BUNDLE_ID_SUFFIX}" "${_plist_suffix}" _plist_contents ${_plist_contents})
		file(WRITE ${_helper_info_plist} ${_plist_contents})

		add_executable(${_helper_target} MACOSX_BUNDLE ${SOURCES})
		SET_EXECUTABLE_TARGET_PROPERTIES(${_helper_target})
		add_dependencies(${_helper_target} libcef_dll_wrapper)
		target_link_libraries(${_helper_target} libcef_dll_wrapper ${CEF_STANDARD_LIBS})
		set_target_properties(${_helper_target} PROPERTIES
			MACOSX_BUNDLE_INFO_PLIST ${_helper_info_plist}
			OUTPUT_NAME ${_helper_output_name})
		
		install(TARGETS ${_helper_target} DESTINATION ${INSTALL_OUT_DIR}/GarrysMod_Signed.app/Contents/Frameworks)
		
		if(USE_SANDBOX)
			target_link_libraries(${_helper_target} cef_sandbox_lib)
		endif()
	endforeach()
endif()
