#include <Windows.h>
#include "include/cef_app.h"
#include "include/cef_sandbox_win.h"

#include "ChromiumApp.h"

int ChromiumMain( HINSTANCE hInstance )
{
    CefRefPtr<ChromiumApp> chromiumApp( new ChromiumApp );

    // Provide CEF with command-line arguments.
    CefMainArgs main_args( hInstance );    

#if defined( _WIN32 ) && defined( NDEBUG )
    CefScopedSandboxInfo info;
    return CefExecuteProcess( main_args, chromiumApp, info.sandbox_info() );
#else
    return CefExecuteProcess( main_args, chromiumApp, nullptr );
#endif
}
