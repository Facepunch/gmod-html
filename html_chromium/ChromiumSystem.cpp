#include "ChromiumSystem.h"
#include "ChromiumClient.h"
#include "ChromiumBrowser.h"
#include "ResourceHandler.h"
#include "HtmlResourceHandler.h"
#include "JSObjects.h"

#include "cef_start.h"
#include "include/cef_app.h"
#include "include/cef_origin_whitelist.h"
#ifdef __APPLE__
	#include "include/wrapper/cef_library_loader.h"
#endif
#include "include/cef_version.h"
#include "cef_end.h"

#include <time.h>
#include <filesystem>
namespace fs = std::filesystem;

class ChromiumApp
	: public CefApp
{
public:
	//
	// CefApp implementation
	//
	void OnBeforeCommandLineProcessing( const CefString& process_type, CefRefPtr<CefCommandLine> command_line ) override
	{
		command_line->AppendSwitch( "enable-gpu" );
		command_line->AppendSwitch( "disable-gpu-compositing" ); // NOTE: Enabling GPU Compositing will make OnAcceleratedPaint run instead of OnPaint (CEF must be patched or NOTHING will run!)
		command_line->AppendSwitch( "disable-smooth-scrolling" );
#ifdef _WIN32
		command_line->AppendSwitch( "enable-begin-frame-scheduling" );
#endif

		// This can interfere with posix signals and break Breakpad
#ifdef __linux__
		command_line->AppendSwitch( "disable-in-process-stack-traces" );

		// Flatpak, AppImage, and Snap break sandboxing
		// GMOD_CEF_NO_SANDBOX is for when we want to FORCE it off
		// TODO(winter): It's not ideal to just outright turn off sandboxing...but Steam does it too, so
		if ( getenv( "GMOD_CEF_NO_SANDBOX" ) || getenv( "container" ) || getenv( "APPIMAGE" ) || getenv( "SNAP" ) ) {
			LOG(WARNING) << "Disabling Chromium sandbox...\n";
			command_line->AppendSwitch("no-sandbox");
		}
#endif

#ifdef __APPLE__
		command_line->AppendSwitch( "use-mock-keychain" );
#endif

		// https://bitbucket.org/chromiumembedded/cef/issues/2400
		// DXVAVideoDecoding must be disabled for Proton/Wine
		// Disable HardwareMediaKeyHandling to prevent external control of media
		command_line->AppendSwitchWithValue( "disable-features", "TouchpadAndWheelScrollLatching,AsyncWheelEvents,DXVAVideoDecoding,HardwareMediaKeyHandling" );

		// Auto-play media
		command_line->AppendSwitchWithValue( "autoplay-policy", "no-user-gesture-required" );

		// Disable site isolation until we implement passing registered Lua functions between processes
		//command_line->AppendSwitch( "disable-site-isolation-trials" );

		// Enable remote debugging; see also: settings.remote_debugging_port
		//command_line->AppendSwitchWithValue( "remote-allow-origins", "http://localhost:9222" );
	}

	void OnRegisterCustomSchemes( CefRawPtr<CefSchemeRegistrar> registrar ) override
	{
		// TODO: are these bools what we want them to be
		registrar->AddCustomScheme( "asset", CEF_SCHEME_OPTION_STANDARD | CEF_SCHEME_OPTION_CSP_BYPASSING );
	}

private:
	IMPLEMENT_REFCOUNTING( ChromiumApp );
};

typedef void* ( *CreateCefSandboxInfoFn )( );
typedef void ( *DestroyCefSandboxInfoFn )( void* );

// TODO: Needs cleaning up. There's too much Windows shit.
bool ChromiumSystem::Init( const char* pBaseDir, IHtmlResourceHandler* pResourceHandler )
{
	g_pHtmlResourceHandler = pResourceHandler;

#ifdef __APPLE__
	static CefScopedLibraryLoader library_loader;
	if ( !library_loader.LoadInMain() )
	{
		return false;
	}
#endif

#if defined( __linux__ ) || defined( __APPLE__ )
	// GMOD: GO - Chromium will replace Breakpad's signal handlers if we don't do this early
	int argc = 2;
	char arg1[] = "binary";
	char arg2[] = "--disable-in-process-stack-traces";
	char* argv[3] = { arg1, arg2, nullptr };
	CefMainArgs main_args( argc, argv );
#else
	CefMainArgs main_args;
#endif
	CefSettings settings;

	std::string strBaseDir = pBaseDir;

	//settings.remote_debugging_port = 9222;
	settings.remote_debugging_port = 0;
	settings.windowless_rendering_enabled = true;

#ifdef CEF_USE_SANDBOX
	settings.no_sandbox = false;
#else
	settings.no_sandbox = true;
#endif

	settings.command_line_args_disabled = true;
	settings.log_severity = LOGSEVERITY_DEFAULT;

#ifdef _WIN32
	std::string platform = "Windows NT";

	// Chromium will be sad if we don't resolve any NTFS junctions for it
	// Is this really the only way Windows will let me do that?
	auto hFile = CreateFileA( strBaseDir.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL );
	bool useTempDir = false;

	if ( hFile != INVALID_HANDLE_VALUE )
	{
		char pathBuf[MAX_PATH] = { 0 };
		if ( GetFinalPathNameByHandleA( hFile, pathBuf, sizeof( pathBuf ), VOLUME_NAME_DOS ) )
		{
			// If it's a network drive, we can't use it
			if ( strstr( pathBuf, "\\\\?\\UNC" ) == pathBuf )
			{
				useTempDir = true;
			}
			else
			{
				strBaseDir.assign( pathBuf + 4 ); // Skipping "\\?\"...
			}
		}

		CloseHandle( hFile );
	}

	std::string chromiumDir = strBaseDir + "/bin/chromium";

	// We've got to copy our CEF resources to a temporary directory and use that instead
	if ( useTempDir )
	{
		pResourceHandler->Message( "Copying Chromium resources to temporary directory...\n" );

		// We have no exceptions, so here is our lovely error handling variable.
		std::error_code error;

		auto tmpPath = fs::temp_directory_path( error );
		if ( error )
			return false;

		auto targetPath = tmpPath / "gmod-resources";

		fs::create_directories( targetPath, error );
		if ( error )
			return false;

		fs::copy( fs::path( strBaseDir ) / "bin/chromium", targetPath, fs::copy_options::recursive | fs::copy_options::overwrite_existing );
		if ( error )
			return false;

		chromiumDir = targetPath.string();
	}

	// GMOD: GO - We use the same resources with 32-bit and 64-bit builds, so always use the 32-bit bin path for them
	// TODO(winter): Disabled since they don't work in CEF 131+ (for now)
	// https://github.com/chromiumembedded/cef/issues/3749
	//CefString( &settings.resources_dir_path ).FromString( chromiumDir );
	//CefString( &settings.locales_dir_path ).FromString( chromiumDir + "/locales" );

	settings.multi_threaded_message_loop = true;
#elif __linux__
	std::string platform = "Linux";

#if defined( __x86_64__ ) || defined( _WIN64 )
	CefString( &settings.browser_subprocess_path ).FromString( strBaseDir + "/bin/linux64/chromium_process" );
#else
	CefString( &settings.browser_subprocess_path ).FromString( strBaseDir + "/bin/linux32/chromium_process" );
#endif

	// GMOD: GO - We use the same resources with 32-bit and 64-bit builds, so always use the 32-bit bin path for them
	// TODO(winter): Disabled since they don't work in CEF 131+ (for now)
	// https://github.com/chromiumembedded/cef/issues/3749
	//CefString( &settings.resources_dir_path ).FromString( strBaseDir + "/bin/linux32/chromium" );
	//CefString( &settings.locales_dir_path ).FromString( strBaseDir + "/bin/linux32/chromium/locales" );

	settings.multi_threaded_message_loop = true;
#elif __APPLE__
	std::string platform = "Macintosh; Intel Mac OS X";
#else
#error
#endif

	std::string chrome_version = std::to_string( CHROME_VERSION_MAJOR ) + ".0.0.0";
	CefString( &settings.user_agent ).FromString( "Mozilla/5.0 (" + platform + "; Valve Source Client) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/" + chrome_version + " Safari/537.36 GMod/13" );

	// Rotate log file
	// TODO(winter): This is probably not the best place to be doing this
	std::error_code rotateError;
	std::string curLogPath = strBaseDir + "/chromium.log";
	std::string lastLogPath = strBaseDir + "/chromium.log.1";

	std::string cefCachePath = strBaseDir + "/ChromiumCache";
	std::string cefLockFilePath = cefCachePath + "/lockfile";

	// Try to delete the lockfile. If we can't, it's still in use. Crashes could leave it behind
	fs::remove( cefLockFilePath );

	if ( fs::exists( cefLockFilePath ) ) {
		pResourceHandler->Message( "Skipping Chromium log rotation (lockfile exists)...\n" );

		unsigned int multirunInstanceID = 0;
		while ( fs::exists( cefCachePath ) && fs::exists( cefLockFilePath ) ) {
			multirunInstanceID++;
			cefCachePath = strBaseDir + "/ChromiumCacheMultirun/" + std::to_string( multirunInstanceID );
			cefLockFilePath = cefCachePath + "/lockfile";
		}

		//m_MultirunCacheDir = cefCachePath;
		std::string m_MultirunCacheDir = cefCachePath;

		std::string tmpCacheMsg = "Using temporary Chromium cache to support multirun: " + m_MultirunCacheDir + "\n";
		pResourceHandler->Message( tmpCacheMsg.c_str() );
	} else {
		fs::copy_file( curLogPath, lastLogPath, fs::copy_options::overwrite_existing, rotateError );

		if ( rotateError ) {
			const std::string rotateErrorMsg = "Couldn't rotate chromium.log (copy): " + rotateError.message() + "\n";
			pResourceHandler->Message( rotateErrorMsg.c_str() );
			rotateError.clear();
		}

		fs::remove( curLogPath, rotateError );

		if ( rotateError ) {
			const std::string rotateErrorMsg = "Couldn't rotate chromium.log (remove): " + rotateError.message() + "\n";
			pResourceHandler->Message( rotateErrorMsg.c_str() );
			rotateError.clear();
		}
	}

	CefString( &settings.log_file ).FromString( curLogPath );

	// CEF 120+ requires this otherwise CEF applications will trample each other
	CefString( &settings.root_cache_path ).FromString( cefCachePath );
	CefString( &settings.cache_path ).FromString( cefCachePath );

	// Grab our Sandbox info from the "game" exe
#if defined( _WIN32 ) && defined( CEF_USE_SANDBOX )
	HMODULE pModule;

	if ( !GetModuleHandleEx( GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, nullptr, &pModule ) )
	{
		return false;
	}

	auto CreateCefSandboxInfo = (CreateCefSandboxInfoFn) GetProcAddress( pModule, "CreateCefSandboxInfo" );
	auto DestroyCefSandboxInfo = (DestroyCefSandboxInfoFn) GetProcAddress( pModule, "DestroyCefSandboxInfo" );

	if ( CreateCefSandboxInfo == nullptr || DestroyCefSandboxInfo == nullptr )
	{
		return false;
	}

	void* sandbox_info = CreateCefSandboxInfo();

	if ( sandbox_info == nullptr )
	{
		return false;
	}
#else
	void* sandbox_info = nullptr;
#endif

	if ( !CefInitialize( main_args, settings, new ChromiumApp, sandbox_info ) )
	{
		pResourceHandler->Message( "CefInitialize failed!\n" );
		return false;
	}

#if defined( _WIN32 ) && defined( CEF_USE_SANDBOX )
	DestroyCefSandboxInfo( sandbox_info );
#endif

	{
		CefRefPtr<ResourceHandlerFactory> factory = new ResourceHandlerFactory;

		CefRegisterSchemeHandlerFactory( "asset", "garrysmod", factory );
		CefAddCrossOriginWhitelistEntry( "asset://garrysmod", "http", "", true );
		CefAddCrossOriginWhitelistEntry( "asset://garrysmod", "https", "", true );
		CefAddCrossOriginWhitelistEntry( "asset://garrysmod", "asset", "", true );

		CefRegisterSchemeHandlerFactory( "asset", "mapimage", factory );
		CefAddCrossOriginWhitelistEntry( "asset://mapimage", "http", "", true );
		CefAddCrossOriginWhitelistEntry( "asset://mapimage", "https", "", true );
		CefAddCrossOriginWhitelistEntry( "asset://mapimage", "asset", "", true );

	}

	{
		CefRefPtr<HtmlResourceHandlerFactory> factory = new HtmlResourceHandlerFactory;

		CefRegisterSchemeHandlerFactory( "asset", "html", factory );
		CefAddCrossOriginWhitelistEntry( "asset://html", "http", "", true );
		CefAddCrossOriginWhitelistEntry( "asset://html", "https", "", true );
		CefAddCrossOriginWhitelistEntry( "asset://html", "asset", "", true );
	}

#ifdef __APPLE__
	CefDoMessageLoopWork();
#endif

	return true;
}

void ChromiumSystem::Shutdown()
{
	CefShutdown();

	// Delete temporary ChromiumCacheMultirun if it exists
	// TODO(winter): For some reason CEF still hasn't released the handles it has for these files even though CefShutdown has finished and the lockfile is gone...
	/*
	if (!m_MultirunCacheDir.empty()) {
		while (fs::exists(m_MultirunCacheDir + "/lockfile")) {
			// Spin until the lockfile is released by CEF
		}

		std::error_code removeTempError;
		fs::remove_all(m_MultirunCacheDir, removeTempError);

		if (removeTempError) {
			const std::string removeTempErrorMsg = "Couldn't remove temporary Chromium cache: " + removeTempError.message() + "\n";
			g_pHtmlResourceHandler->Message(removeTempErrorMsg.c_str());
		}
	}
	*/
}

IHtmlClient* ChromiumSystem::CreateClient( IHtmlClientListener* listener )
{
	CefWindowInfo windowInfo;
	windowInfo.SetAsWindowless( 0 );
#ifdef _WIN32
	//windowInfo.shared_texture_enabled = true;
#endif

	CefBrowserSettings browserSettings;
	CefString( &browserSettings.default_encoding ).FromString( "UTF-8" );
	browserSettings.windowless_frame_rate = 60;
	browserSettings.javascript_access_clipboard = STATE_DISABLED;
	browserSettings.javascript_close_windows = STATE_DISABLED;
	browserSettings.webgl = STATE_ENABLED;

	CefRefPtr<ChromiumBrowser> cefClient( new ChromiumBrowser() );

	// Queue the browser creation. It's async, but ChromiumBrowser will handle it all.
	CefBrowserHost::CreateBrowser( windowInfo, cefClient, "", browserSettings, nullptr, nullptr );

	return new ChromiumClient( cefClient, listener );
}

void ChromiumSystem::Update()
{
	// Run any asset:// requests that are pending
	m_RequestsLock.Acquire();
	{
		for ( auto& request : m_Requests )
		{
			request->Handle();
		}

		m_Requests = std::vector<CefRefPtr<ResourceHandler>>();
	}
	m_RequestsLock.Release();

	// macOS will want me
#ifdef __APPLE__
	CefDoMessageLoopWork();
#endif

	// This is where messages from the browser UI thread come to the main thread
	for ( auto client : m_ActiveClients )
	{
		client->Update();
	}
}

void ChromiumSystem::OnClientOpen( ChromiumClient* client )
{
	m_ActiveClients.emplace_back( client );
}

void ChromiumSystem::OnClientClose( ChromiumClient* client )
{
	m_ActiveClients.erase( std::remove( m_ActiveClients.begin(), m_ActiveClients.end(), client ), m_ActiveClients.end() );
}

void ChromiumSystem::QueueRequest( CefRefPtr<ResourceHandler> resourceHandler )
{
	m_RequestsLock.Acquire();
	m_Requests.emplace_back( resourceHandler );
	m_RequestsLock.Release();
}

JSValue ChromiumSystem::CreateArray( JSValue* pValues, size_t size )
{
	std::vector<JSValue> container( size );

	for ( size_t i = 0; i < size; i++ )
	{
		container[i] = std::move( pValues[i] );
	}

	return JSArray::Create( std::move( container ) );
}

JSValue ChromiumSystem::CreateString( const char* pValue, size_t size )
{
	return JSString::Create( std::string( pValue, size ) );
}

JSValue ChromiumSystem::CreateHashMap( const char** pKeys, const size_t* pKeySizes, JSValue* pValues, size_t size )
{
	std::unordered_map<std::string, JSValue> container;
	container.reserve( size );

	// Last value takes precedence if there are any duplicate keys
	for ( size_t i = 0; i < size; i++ )
	{
		container[pKeys[i]] = std::move( pValues[i] );
	}

	return JSHashMap::Create( std::move( container ) );
}

ChromiumSystem g_ChromiumSystem;
HTMLSYSTEM_EXPORT( g_ChromiumSystem );
