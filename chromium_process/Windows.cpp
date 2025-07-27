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

typedef int (*LauncherMain_t)(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);

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

	// TODO: Unicode paths (GetModuleFileNameW, etc)
	char executable_path[MAX_PATH] = { 0 };
	GetModuleFileNameA(NULL, executable_path, MAX_PATH);

	std::string::size_type last_slash = std::string(executable_path).find_last_of("\\/");
	std::string executable_dir = std::string(executable_path).substr(0, last_slash);

#ifdef ENVIRONMENT64
	executable_dir += "\\..\\..";
#else
	executable_dir += "\\..";
	//MessageBoxA(NULL, "You may encounter stability issues with Garry's Mod in 32-bit.\n\nPlease consider using the x86-64 beta and launching in 64-bit mode if possible.", "32-bit Warning", 0);
#endif

	// NOTE: GMod's main branch does _chdir in launcher.dll, which will override this!
	// We work around it in gmod_32bit_redirector by using -basedir
	_chdir(executable_dir.c_str());

	// Launch GarrysMod's main function from this process. We needed this so the "main" process could provide sandbox information above.
	HMODULE hLauncher = LoadLibraryA("launcher.dll");
	LauncherMain_t mainFn = (LauncherMain_t)(GetProcAddress(hLauncher, "LauncherMain"));

	if (!mainFn) {
		DWORD err = GetLastError();
		LPVOID err_msg;

		int err_format_result = FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			err,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(LPWSTR) &err_msg,
			0,
			NULL
		);

		if (err_format_result == 0) {
			MessageBoxW(NULL, L"<Couldn't format error message>", L"Launch Error: GetProcAddress", MB_ICONERROR);
		} else {
			MessageBoxW(NULL, (LPWSTR) err_msg, L"Launch Error: GetProcAddress", MB_ICONERROR);
			LocalFree(err_msg);
		}

		return err;
	}

	return mainFn(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
}
