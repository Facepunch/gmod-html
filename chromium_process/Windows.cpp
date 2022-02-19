#include <Windows.h>
#include "include/cef_app.h"

#ifdef CEF_USE_SANDBOX
	#include "include/cef_sandbox_win.h"
#endif

#include "ChromiumApp.h"

int ChromiumMain(HINSTANCE hInstance)
{
	CefEnableHighDPISupport();

	void* sandbox_info = nullptr;

#ifdef CEF_USE_SANDBOX
	CefScopedSandboxInfo scoped_sandbox;
	sandbox_info = scoped_sandbox.sandbox_info();
#endif

	CefMainArgs main_args(hInstance);
	CefRefPtr<ChromiumApp> app(new ChromiumApp());

	return CefExecuteProcess(main_args, app, sandbox_info);
}
