
#include <cstdlib>
#include <string>
#include <iostream>

#include <Windows.h>
#include <shlwapi.h>
#include <direct.h>

#if __x86_64__ || _WIN64
	#define ENVIRONMENT64
#else
	#define ENVIRONMENT32
#endif

#include "include/cef_app.h"
#include "ChromiumApp.h"

#ifdef CEF_USE_SANDBOX
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

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) {
	// Check if "--type=" is in the command arguments. If it is, we are a chromium subprocess.
	if (strstr(lpCmdLine, "--type=")) {
		void* sandbox_info = nullptr;

#ifdef CEF_USE_SANDBOX
		CefScopedSandboxInfo scoped_sandbox;
		sandbox_info = scoped_sandbox.sandbox_info();
#endif

		CefMainArgs main_args(hInstance);
		CefRefPtr<ChromiumApp> app(new ChromiumApp());

		int exit_code = CefExecuteProcess(main_args, app, sandbox_info);
		if (exit_code >= 0) {
			return exit_code;
		}
	}

	char executable_path[MAX_PATH] = { 0 };
	GetModuleFileNameA(NULL, executable_path, MAX_PATH);

	std::string::size_type last_slash = std::string(executable_path).find_last_of("\\/");
	std::string executable_dir = std::string(executable_path).substr(0, last_slash);

#ifdef ENVIRONMENT64
	executable_dir += "\\..\\..";
#else
	executable_dir += "\\..";
	MessageBoxA(NULL, "You may encounter stability issues with GModCEFCodecFix in 32-bit mode. Please launch Garry's Mod in 64-bit mode instead if possible.", "32-bit Warning", 0);
#endif

	_chdir(executable_dir.c_str());

	// Launch GarrysMod's main function from this process. We needed this so the "main" process could provide sandbox information above.
	HMODULE hLauncher = LoadLibraryA("launcher.dll");
	void* mainFn = static_cast<void*>(GetProcAddress(hLauncher, "LauncherMain"));
	return (static_cast<FuncLauncherMain>(mainFn))(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
}
