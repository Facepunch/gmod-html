#include "include/cef_app.h"
#include "ChromiumApp.h"

int main( int argc, char* argv[] )
{
    CefRefPtr<ChromiumApp> chromiumApp( new ChromiumApp );

    // Provide CEF with command-line arguments.
    CefMainArgs main_args( argc, argv );    

    // CEF applications have multiple sub-processes (render, plugin, GPU, etc)
    // that share the same executable. This function checks the command-line and,
    // if this is a sub-process, executes the appropriate logic.
    return CefExecuteProcess( main_args, chromiumApp, nullptr );
}
