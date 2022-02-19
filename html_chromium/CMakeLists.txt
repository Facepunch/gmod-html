
set(TARGET html_chromium)
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

if(OS_LINUX OR OS_WINDOWS)
  ADD_LOGICAL_TARGET("libcef_lib" "${CEF_LIB_DEBUG}" "${CEF_LIB_RELEASE}")
endif()
	
add_library(${TARGET} SHARED ${SOURCES})
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
if(OS_MAC)
	set_target_properties(${TARGET} PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CEF_TARGET_OUT_DIR}/GarrysMod_Signed.app/Contents/MacOS")
endif()