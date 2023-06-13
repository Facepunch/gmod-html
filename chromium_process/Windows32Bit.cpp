#include <Windows.h>

// Disable 32-bit Windows (for now?), since CEF 114 crashes on it while joining a server
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow) {
	MessageBoxA(NULL, "GModCEFCodecFix doesn't support 32-bit. Please launch Garry's Mod in 64-bit mode.", "Launcher Error", 0);
	return 0;
}
