set(SOURCES
	Main.cpp)

if(WIN32)
	set(SOURCES
		${SOURCES}
		../html_chromium/chromium_process/ChromiumApp.cpp
		../html_chromium/chromium_process/ChromiumApp.h
		../html_chromium/chromium_process/Windows.cpp)
endif()

# Lame
include_directories(${CEF_PATH})

add_executable(gmod_launcher WIN32 ${SOURCES})
target_link_libraries(gmod_launcher html libcef_imp libcef_dll_wrapper)
target_link_libraries(gmod_launcher optimized cef_sandbox)

if(WIN32)
	target_link_libraries(gmod_launcher
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
