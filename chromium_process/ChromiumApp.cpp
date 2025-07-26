#include "ChromiumApp.h"

static bool V8ValueToCefValue( CefRefPtr<CefValue> outValue, const CefRefPtr<CefV8Value>& inValue, int depth = 0 )
{
	if ( depth > 16 )
		return false;

	if ( inValue->IsNull() || inValue->IsUndefined() )
	{
		outValue->SetNull();
	}
	else if ( inValue->IsBool() )
	{
		outValue->SetBool( inValue->GetBoolValue() );
	}
	else if ( inValue->IsInt() || inValue->IsUInt() || inValue->IsDouble() )
	{
		outValue->SetDouble( inValue->GetDoubleValue() );
	}
	else if ( inValue->IsString() )
	{
		outValue->SetString( inValue->GetStringValue() );
	}
	else if ( inValue->IsArray() )
	{
		int arrayLen = inValue->GetArrayLength();
		auto newList = CefListValue::Create();
		newList->SetSize( arrayLen );

		for ( int i = 0; i < arrayLen; i++ )
		{
			auto newValue = CefValue::Create();

			if ( !V8ValueToCefValue( newValue, inValue->GetValue( i ), depth + 1 ) )
				return false;

			newList->SetValue( i, newValue );
		}

		outValue->SetList( newList );
	}
	else if ( inValue->IsObject() )
	{
		auto newMap = CefDictionaryValue::Create();
		std::vector<CefString> keys;
		inValue->GetKeys( keys );

		for ( auto const& key : keys )
		{
			auto newValue = CefValue::Create();

			if ( !V8ValueToCefValue( newValue, inValue->GetValue( key ), depth + 1 ) )
				return false;

			newMap->SetValue( key, newValue );
		}

		outValue->SetDictionary( newMap );
	}
	else
	{
		outValue->SetNull();
	}

	return true;
}

static bool V8ValuesToCefList( CefRefPtr<CefListValue>& outList, const CefV8ValueList& inList )
{
	outList->SetSize( inList.size() );

    size_t index = 0;
    for ( const auto& x : inList )
    {
		auto newValue = CefValue::Create();

		if ( !V8ValueToCefValue( newValue, x ) )
			return false;

		outList->SetValue( index, newValue );
		index++;
    }

    return true;
}

static bool CefValueToV8Value( CefRefPtr<CefV8Value>& outValue, const CefRefPtr<CefValue>& inValue, int depth = 0 )
{
	if ( depth > 16 )
		return false;

	switch ( inValue->GetType() )
	{
		case VTYPE_NULL:
			outValue = CefV8Value::CreateUndefined();
			break;
		case VTYPE_BOOL:
			outValue = CefV8Value::CreateBool( inValue->GetBool() );
			break;
		case VTYPE_DOUBLE:
			outValue = CefV8Value::CreateDouble( inValue->GetDouble() );
			break;
		case VTYPE_STRING:
			outValue = CefV8Value::CreateString( inValue->GetString() );
			break;
		case VTYPE_LIST:
		{
			auto inList = inValue->GetList();
			int size = static_cast<int>( inList->GetSize() );
			outValue = CefV8Value::CreateArray( size );

			for ( int i = 0; i < size; i++ )
			{
				CefRefPtr<CefV8Value> entry;

				if ( !CefValueToV8Value( entry, inList->GetValue( i ), depth + 1 ) )
					return false;

				outValue->SetValue( i, entry );
			}
			break;
		}
		case VTYPE_DICTIONARY:
		{
			auto inMap = inValue->GetDictionary();
			outValue = CefV8Value::CreateObject( nullptr, nullptr );

			CefDictionaryValue::KeyList keys;
			inMap->GetKeys( keys );

			for ( const auto& key : keys )
			{
				CefRefPtr<CefV8Value> value;

				if ( !CefValueToV8Value( value, inMap->GetValue( key ), depth + 1 ) )
					return false;

				outValue->SetValue( key, value, V8_PROPERTY_ATTRIBUTE_NONE );
			}
			break;
		}
		default:
			outValue = CefV8Value::CreateUndefined();
			break;
	}

	return true;
}

static bool CefListToV8Values( CefV8ValueList& outList, const CefRefPtr<CefListValue> inList )
{
	outList.reserve( inList->GetSize() );

	for ( size_t i = 0; i < inList->GetSize(); i++ )
	{
		CefRefPtr<CefV8Value> entry;

		if ( !CefValueToV8Value( entry, inList->GetValue( i ) ) )
			return false;

		outList.emplace_back( entry );
	}

	return true;
}

//
// CefApp interface
//
void ChromiumApp::OnBeforeCommandLineProcessing( const CefString& process_type, CefRefPtr<CefCommandLine> command_line )
{
    command_line->AppendSwitch( "disable-gpu" );
    command_line->AppendSwitch( "disable-gpu-compositing" );
    command_line->AppendSwitch( "disable-smooth-scrolling" );
#ifdef _WIN32
    command_line->AppendSwitch( "enable-begin-frame-scheduling" );
#endif
    command_line->AppendSwitch( "enable-system-flash" );

    // This can interfere with posix signals and break Breakpad
#if defined( __linux__ ) || defined( __APPLE__ )
    command_line->AppendSwitch( "disable-in-process-stack-traces" );

	// Flatpak, AppImage, and Snap break sandboxing
	// GMOD_CEF_NO_SANDBOX is for when we want to FORCE it off
	// TODO(winter): It's not ideal to just outright turn off sandboxing...but Steam does it too, so
	if ( getenv( "GMOD_CEF_NO_SANDBOX" ) || getenv( "container" ) || getenv( "APPIMAGE" ) || getenv( "SNAP" )) {
		LOG(WARNING) << "Disabling Chromium sandbox...\n";
		command_line->AppendSwitch( "no-sandbox" );
	}
#endif

#ifdef __APPLE__
        command_line->AppendSwitch( "use-mock-keychain" );
#endif

    // https://bitbucket.org/chromiumembedded/cef/issues/2400
	// DXVAVideoDecoding must be disabled for Proton/Wine
    command_line->AppendSwitchWithValue( "disable-features", "TouchpadAndWheelScrollLatching,AsyncWheelEvents,DXVAVideoDecoding" );

    // Auto-play media
    command_line->AppendSwitchWithValue( "autoplay-policy", "no-user-gesture-required" );

    // Chromium 80 removed this but only sometimes.
    command_line->AppendSwitchWithValue( "enable-blink-features", "HTMLImports" );

    // Disable site isolation until we implement passing registered Lua functions between processes
    command_line->AppendSwitch( "disable-site-isolation-trials" );
}

void ChromiumApp::OnRegisterCustomSchemes( CefRawPtr<CefSchemeRegistrar> registrar )
{
    // TODO: are these bools what we want them to be?
    registrar->AddCustomScheme( "asset", CEF_SCHEME_OPTION_STANDARD | CEF_SCHEME_OPTION_CSP_BYPASSING );
}

CefRefPtr<CefRenderProcessHandler> ChromiumApp::GetRenderProcessHandler()
{
    return this;
}

//
// CefBrowserProcessHandler interface
//
bool ChromiumApp::OnAlreadyRunningAppRelaunch( CefRefPtr<CefCommandLine> command_line, const CefString &current_directory )
{
	// See ChromiumSystem::Init, we detect lockfile and handle things there
	return true;
}

//
// CefRenderProcessHandler interface
//
void ChromiumApp::OnContextCreated( CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context )
{
    //
    // CEF3 only supports implementing CefPrintHandler on Linux, so we've gotta just remove window.print.
    //
    context->Enter();
    {
        context->GetGlobal()->DeleteValue( "print" );
    }
    context->Exit();

    // If this is a web worker, we want nothing to do with it
    if ( !browser )
        return;
    
    for ( auto& pair : m_RegisteredFunctions )
    {
        RegisterFunctionInFrame( frame, pair.first, pair.second );
    }
}

void ChromiumApp::OnContextReleased( CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context )
{
	// Do nothing
}

bool ChromiumApp::OnProcessMessageReceived( CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefProcessId source_process, CefRefPtr<CefProcessMessage> message )
{
    auto name = message->GetName();
    auto args = message->GetArgumentList();

    if ( name == "RegisterFunction" )
    {
        RegisterFunction( browser, args );
        return true;
    }

    if ( name == "ExecuteCallback" )
    {
        ExecuteCallback( browser, args );
        return true;
    }

    if ( name == "ForgetCallback" )
    {
        ForgetCallback( browser, args );
        return true;
    }

	if ( name == "ExecuteJavaScript" )
	{
		ExecuteJavaScript( browser, args );
		return true;
	}

    return false;
}

//
// CefV8Handler interface
//
bool ChromiumApp::Execute( const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception )
{
    auto context = CefV8Context::GetCurrentContext();
    auto browser = context->GetBrowser();

    if ( !browser )
    {
        // This could happen inside of a web worker (but our functions shouldn't be registered there)
        exception = "CefV8Context::GetBrowser == nullptr";
        return true;
    }

    //
    // We need to rip the objName/funcName out of this function's name
    //
    std::string objName, funcName;
    {
        std::string fullName = name.ToString();
        size_t delimPos = fullName.find( '#' );

        if ( delimPos == std::string::npos )
        {
            // Shouldn't happen
            exception = "ChromiumApp::Execute couldn't parse this function's name";
            return true;
        }

        objName = fullName.substr( 0, delimPos );
        funcName = fullName.substr( delimPos + 1 );
    }

	auto argsList = CefListValue::Create();
	if ( !V8ValuesToCefList( argsList, arguments ) )
	{
		exception = "failed to serialize parameters (did you return a deeply nested structure?)";
		return true;
	}

    CefRefPtr<CefV8Value> callback;

    auto message = CefProcessMessage::Create( "ExecuteFunction" );
    auto args = message->GetArgumentList();

    args->SetString( 0, objName );
    args->SetString( 1, funcName );

    // No callback = easy mode
    if ( arguments.empty() || !arguments.back()->IsFunction() )
    {
        args->SetInt( 2, -1 ); // Invalid callback id
        args->SetList( 3, argsList );
    }
    else
    {
        // Now register a callback index that Lua will send back to us later
        int callbackId = CreateCallback( context, arguments.back() );

        // We pass argsList to Lua, so pop the callback off of it
		argsList->Remove( argsList->GetSize() - 1 );

        args->SetInt( 2, callbackId );
        args->SetList( 3, argsList );
    }

    browser->GetMainFrame()->SendProcessMessage( PID_BROWSER, message );
    return true;
}

//

int ChromiumApp::CreateCallback( CefRefPtr<CefV8Context> context, CefRefPtr<CefV8Value> func )
{
    if ( m_Callbacks.find( m_NextCallbackId ) != m_Callbacks.end() )
    {
        // We're overlapping? Probably shouldn't happen
        return -1;
    }

    int callbackId = m_NextCallbackId;
    Callback cb;
    cb.Context = context;
    cb.Function = func;

    m_Callbacks.emplace( callbackId, cb );

    m_NextCallbackId++;
    if ( m_NextCallbackId >= 16384 )
        m_NextCallbackId = 0;

    return callbackId;
}

void ChromiumApp::RegisterFunctionInFrame( CefRefPtr<CefFrame> frame, const CefString& objName, const CefString& funcName )
{
    if ( !frame )
        return;

    auto context = frame->GetV8Context();

    if ( !context )
        return;

    //
    // We can only associate one string with a function in CEF. So we'll use "{objName}#{funcName}".
    //
    CefString fullName;
    {
        std::stringstream ss;
        ss << objName.ToString();
        ss << '#';
        ss << funcName.ToString();
        fullName = CefString( ss.str() );
    }

    context->Enter();
    {
        auto window = context->GetGlobal();
        auto obj = window->GetValue( objName );

        // If our object doesn't exist, create it
        if ( !obj || !obj->IsObject() )
        {
            obj = CefV8Value::CreateObject( nullptr, nullptr );
            window->SetValue( objName, obj, V8_PROPERTY_ATTRIBUTE_NONE );
        }

        auto func = CefV8Value::CreateFunction( fullName, this );
        obj->SetValue( funcName, func, V8_PROPERTY_ATTRIBUTE_NONE );
    }
    context->Exit();
}

void ChromiumApp::ExecuteJavaScript( CefRefPtr<CefBrowser> browser, CefRefPtr<CefListValue> args )
{
	CefString jsUrl = args->GetString( 0 );
	CefString jsSrc = args->GetString( 1 );

	auto mainFrame = browser->GetMainFrame();

	if ( !mainFrame || !mainFrame->IsValid() )
		return;

	auto context = mainFrame->GetV8Context();

	if ( !context || !context->IsValid() )
		return;

	context->Enter();
	{
		CefRefPtr<CefV8Value> retVal;
		CefRefPtr<CefV8Exception> retException;
		context->Eval( jsSrc, jsUrl, 1, retVal, retException );
	}
	context->Exit();
}

void ChromiumApp::RegisterFunction( CefRefPtr<CefBrowser> browser, CefRefPtr<CefListValue> args )
{
    CefString objName = args->GetString( 0 );
    CefString funcName = args->GetString( 1 );

    // Register this function in any frames that already exist
    {
        std::vector<CefString> frames;
        browser->GetFrameIdentifiers( frames );

        for ( auto frameId : frames )
        {
            RegisterFunctionInFrame( browser->GetFrameByIdentifier( frameId ), objName, funcName );
        }
    }

    m_RegisteredFunctions.emplace_back( std::make_pair( objName, funcName ) );
}

void ChromiumApp::ExecuteCallback( CefRefPtr<CefBrowser> browser, CefRefPtr<CefListValue> args )
{
    int callbackId = args->GetInt( 0 );
    auto it = m_Callbacks.find( callbackId );
    if ( it == m_Callbacks.end() )
        return;

    auto context = it->second.Context;
    auto func = it->second.Function;

    m_Callbacks.erase( it );

    // Context has been destroyed for some reason
    if ( !context->IsValid() )
        return;

    auto argList = args->GetList( 1 );

    context->Enter();
    {
		CefV8ValueList arguments;
		if ( CefListToV8Values( arguments, argList ) )
		{
			func->ExecuteFunction( nullptr, arguments );
		}        
    }
    context->Exit();
}

void ChromiumApp::ForgetCallback( CefRefPtr<CefBrowser> browser, CefRefPtr<CefListValue> args )
{
    m_Callbacks.erase( args->GetInt( 0 ) );
}
