#include "include/cef_app.h"
#include "ChromiumApp.h"

int main(int argc, char* argv[]) {
	CefMainArgs main_args(argc, argv);
	CefRefPtr<ChromiumApp> app(new ChromiumApp());
	return CefExecuteProcess(main_args, app, nullptr);
}
