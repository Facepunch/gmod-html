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

if(WIN32)
	set(SOURCES
		${SOURCES}
		../html_chromium/chromium_process/ChromiumApp.cpp
		../html_chromium/chromium_process/ChromiumApp.h
		../html_chromium/chromium_process/Windows.cpp)
endif()

# Lame
include_directories(${CEF_PATH})

add_executable(example_host WIN32 ${SOURCES})
target_link_libraries(example_host glfw glad imgui html libcef_imp libcef_dll_wrapper)
target_link_libraries(example_host optimized cef_sandbox)

if(WIN32)
	target_link_libraries(example_host
		shlwapi
		winmm
		wsock32
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
endif()
