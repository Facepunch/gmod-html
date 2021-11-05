#include <Windows.h>
#include <shlwapi.h>

#include <cstdlib>
#include <string>

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

	std::string env{std::getenv("PATH")};
	env += ";D:\\Steam\\steamapps\\common\\GarrysMod\\bin\\win64";
	SetEnvironmentVariable("PATH", env.c_str());

	HMODULE hLauncher = LoadLibraryA("launcher.dll");
	void* mainFn = static_cast<void*>(GetProcAddress(hLauncher, "LauncherMain"));
	(static_cast<FuncLauncherMain>(mainFn))(hInstance, hPrevInstance, lpCmdLine, nCmdShow);

	return 0;
}