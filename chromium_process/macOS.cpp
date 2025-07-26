#include "include/cef_app.h"
#include "include/wrapper/cef_library_loader.h"

#if defined(CEF_USE_SANDBOX)
#include "include/cef_sandbox_mac.h"
#endif

#include "ChromiumApp.h"

// Entry point function for sub-processes.
int main( int argc, char* argv[] ) {
  // Initialize the macOS sandbox for this helper process.
#if defined(CEF_USE_SANDBOX)
  CefScopedSandboxContext sandbox_context;
  if ( !sandbox_context.Initialize( argc, argv ) )
    return 1;
#endif

  // Load the CEF framework library at runtime instead of linking directly
  // as required by the macOS sandbox implementation.
  CefScopedLibraryLoader library_loader;
  if ( !library_loader.LoadInHelper() )
    return 1;

  CefRefPtr<ChromiumApp> chromiumApp( new ChromiumApp );

  // Provide CEF with command-line arguments.
  CefMainArgs main_args( argc, argv );

  // Execute the sub-process.
  return CefExecuteProcess( main_args, chromiumApp, nullptr );
}
