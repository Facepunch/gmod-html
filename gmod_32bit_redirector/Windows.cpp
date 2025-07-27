#include <Windows.h>
#include <shlwapi.h>
#include <iostream>

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) {
	// NOTE: GMod's main branch does _chdir in launcher.dll, which can be overriden by -basedir
	std::string newlpCmdLine = lpCmdLine;
	if (!strstr(lpCmdLine, "-basedir")) {
		newlpCmdLine = "-basedir \"\\..\" ";
		newlpCmdLine += lpCmdLine;
	}

	// Figure out the target executable path
	char current_dir[MAX_PATH];
	GetModuleFileNameA(NULL, current_dir, MAX_PATH);
	PathRemoveFileSpecA(current_dir);
	std::string executable_dir = std::string(current_dir);
	std::string executable_path = std::string(current_dir) + "\\bin\\gmod.exe";
	std::string executable_cmdline = "\"" + executable_path + "\" " + newlpCmdLine;

	// Gather some more info
	STARTUPINFOA si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	// Launch the real process
	if (!CreateProcessA(executable_path.c_str(), const_cast<char *>(executable_cmdline.c_str()), NULL, NULL, false, 0, NULL, NULL, &si, &pi)) {
		DWORD err = GetLastError();
		LPVOID err_msg;

		int err_format_result = FormatMessageA(
			FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			err,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			(char *) &err_msg,
			0,
			NULL
		);

		if (err_format_result == 0) {
			MessageBoxA(NULL, "<Couldn't format error message>", "Launch Error: CreateProcess", MB_ICONERROR);
		} else {
			MessageBoxA(NULL, (char *)err_msg, "Launch Error: CreateProcess", MB_ICONERROR);
			LocalFree(err_msg);
		}

		return err;
	}

	// Clean up afterwards
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	return 0;
}
