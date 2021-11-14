#include <Windows.h>
#include <shlwapi.h>

#include <cstdlib>
#include <string>
#include <iostream>

#ifdef _WIN32
	#include <direct.h>
	#define cd _chdir
#else
	#include "unistd.h"
	#define cd chdir
#endif

// Is this exe 32-bit or 64-bit?
#if _WIN32 || _WIN64
	#if _WIN64
		#define ENVIRONMENT64
	#else
		#define ENVIRONMENT32
	#endif
#endif
#if __GNUC__
	#if __x86_64__ || __ppc64__
		#define ENVIRONMENT64
	#else
		#define ENVIRONMENT32
	#endif
#endif

#if defined( _WIN32 )
	#include "include/cef_sandbox_win.h"

	extern "C"
	{
		__declspec( dllexport ) void* CreateCefSandboxInfo()
		{
			return cef_sandbox_info_create();
		}

		__declspec( dllexport ) void DestroyCefSandboxInfo( void* info )
		{
			cef_sandbox_info_destroy( info );
		}
	}
#endif

using FuncLauncherMain = int (*)(HINSTANCE, HINSTANCE, LPSTR, int);

int WINAPI WinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow )
{
	// Sub-process
#ifdef _WIN32
	if ( strstr( lpCmdLine, "--type=" ) )
	{
		int ChromiumMain( HINSTANCE hInstance );

		int exit_code = ChromiumMain( hInstance );

		if ( exit_code != -1 )
		{
			return exit_code;
		}
	}
#endif

	// Make sure the CWD is always GarrysMod root (where hl2.exe is)
	char exePath[MAX_PATH];
	GetModuleFileNameA(NULL, exePath, MAX_PATH);

	std::string::size_type lastSlashPos = std::string(exePath).find_last_of("\\/");
	std::string exeDir = std::string(exePath).substr(0, lastSlashPos);

#ifdef ENVIRONMENT64
	exeDir += "\\..\\..";
#else
	exeDir += "\\..";
#endif

	cd(exeDir.c_str());

	HMODULE hLauncher = LoadLibraryA("launcher.dll");
	void* mainFn = static_cast<void*>(GetProcAddress(hLauncher, "LauncherMain"));
	(static_cast<FuncLauncherMain>(mainFn))(hInstance, hPrevInstance, lpCmdLine, nCmdShow);

	return 0;
}
