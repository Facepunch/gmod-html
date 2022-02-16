#include "ChromiumBrowser.h"

#include "html/IHtmlClient.h"
#include "JSObjects.h"

#include "cef_start.h"
#include "include/cef_parser.h"
#include "cef_end.h"

static bool CefValueToJSValue( JSValue& outValue, CefRefPtr<CefValue> inValue, int depth = 0 )
{
	if ( depth > 16 )
		return false;

	switch ( inValue->GetType() )
	{
		case VTYPE_NULL:
			outValue = JSValue();
			break;
		case VTYPE_BOOL:
			outValue = JSValue( inValue->GetBool() );
			break;
		case VTYPE_DOUBLE:
			outValue = JSValue( inValue->GetDouble() );
			break;
		case VTYPE_STRING:
			outValue = JSString::Create( std::string( inValue->GetString() ) );
			break;
		case VTYPE_LIST:
		{
			auto inList = inValue->GetList();
			std::vector<JSValue> outList;
			outList.reserve( inList->GetSize() );

			for ( size_t i = 0; i < inList->GetSize(); i++ )
			{
				JSValue entry;

				if ( !CefValueToJSValue( entry, inList->GetValue( i ), depth + 1 ) )
					return false;

				outList.emplace_back( entry );
			}

			outValue = JSArray::Create( std::move( outList ) );
			break;
		}
		case VTYPE_DICTIONARY:
		{
			auto inMap = inValue->GetDictionary();
			std::unordered_map<std::string, JSValue> outMap;
			outMap.reserve( inMap->GetSize() );

			CefDictionaryValue::KeyList keys;
			inMap->GetKeys( keys );

			for ( const auto& key : keys )
			{
				JSValue value;

				if ( !CefValueToJSValue( value, inMap->GetValue( key ), depth + 1 ) )
					return false;

				outMap.emplace( key, value );
			}

			outValue = JSHashMap::Create( std::move( outMap ) );
			break;
		}
		default:
			outValue = JSValue();
			break;
	}

	return true;
}

static bool CefListToJSValues( std::vector<JSValue>& outList, const CefRefPtr<CefListValue> inList )
{
	outList.reserve( inList->GetSize() );

	for ( size_t i = 0; i < inList->GetSize(); i++ )
	{
		JSValue entry;

		if ( !CefValueToJSValue( entry, inList->GetValue( i ) ) )
			return false;

		outList.emplace_back( entry );
	}

	return true;
}

static bool JSValueToCefValue( CefRefPtr<CefValue>& outValue, const JSValue& inValue, int depth = 0 )
{
	if ( depth > 16 )
		return false;

	switch ( inValue.GetType() )
	{
		case JSValue::Type::Undefined:
			outValue->SetNull();
			break;
		case JSValue::Type::Bool:
			outValue->SetBool( inValue.GetBool() );
			break;
		case JSValue::Type::Number:
			outValue->SetDouble( inValue.GetNumber() );
			break;
		case JSValue::Type::String:
		{
			const auto& string = ( static_cast<const JSString*>( inValue.GetInternalString() ) )->GetInternalData(); // spaghetti
			outValue->SetString( string );
			break;
		}
		case JSValue::Type::Array:
		{
			const auto& array = ( static_cast<const JSArray*>( inValue.GetInternalArray() ) )->GetInternalData(); // spaghetti
			auto newList = CefListValue::Create();
			newList->SetSize( array.size() );

			size_t i = 0;
			for ( const auto& x : array )
			{
				auto newValue = CefValue::Create();

				if ( !JSValueToCefValue( newValue, x, depth + 1 ) )
					return false;

				newList->SetValue( i, newValue );
				i++;
			}

			outValue->SetList( newList );
			break;
		}
		case JSValue::Type::HashMap:
		{
			const auto& hashmap = ( static_cast<const JSHashMap*>( inValue.GetInternalHashMap() ) )->GetInternalData(); // spaghetti

			auto newMap = CefDictionaryValue::Create();

			for ( const auto& x : hashmap )
			{
				auto value = CefValue::Create();

				if ( !JSValueToCefValue( value, x.second, depth + 1 ) )
					return false;

				newMap->SetValue( x.first, value );
			}

			outValue->SetDictionary( newMap );
			break;
		}
		default:
			outValue->SetNull();
			break;
	}

	return true;
}

static bool JSValuesToCefList( CefRefPtr<CefListValue> outList, const std::vector<JSValue>& inList )
{
	outList->SetSize( inList.size() );

	size_t index = 0;
	for ( const auto& x : inList )
	{
		auto newValue = CefValue::Create();

		if ( !JSValueToCefValue( newValue, x ) )
			return false;

		outList->SetValue( index, newValue );
		index++;
	}

	return true;
}

static int GetModifiers( const IHtmlClient::EventModifiers modifiers )
{
	int gameModifiers = static_cast<int>( modifiers );
	int chromiumModifiers = 0;

	if ( gameModifiers & static_cast<int>( IHtmlClient::EventModifiers::Shift ) )
		chromiumModifiers |= EVENTFLAG_SHIFT_DOWN;

	if ( gameModifiers & static_cast<int>( IHtmlClient::EventModifiers::Control ) )
		chromiumModifiers |= EVENTFLAG_CONTROL_DOWN;

	if ( gameModifiers & static_cast<int>( IHtmlClient::EventModifiers::Alt ) )
		chromiumModifiers |= EVENTFLAG_ALT_DOWN;

	if ( gameModifiers & static_cast<int>( IHtmlClient::EventModifiers::LeftMouse ) )
		chromiumModifiers |= EVENTFLAG_LEFT_MOUSE_BUTTON;

	if ( gameModifiers & static_cast<int>( IHtmlClient::EventModifiers::MiddleMouse ) )
		chromiumModifiers |= EVENTFLAG_MIDDLE_MOUSE_BUTTON;

	if ( gameModifiers & static_cast<int>( IHtmlClient::EventModifiers::RightMouse ) )
		chromiumModifiers |= EVENTFLAG_RIGHT_MOUSE_BUTTON;

	return chromiumModifiers;
}

ChromiumBrowser::ChromiumBrowser()
	: m_Wide( 512 )
	, m_Tall( 512 )
	, m_PopupWide( 0 )
	, m_PopupTall( 0 )
	, m_PopupData( nullptr )
	, m_OpenLinksExternally( false )
{}

ChromiumBrowser::~ChromiumBrowser()
{
	delete[] m_PopupData;
}

ImageData& ChromiumBrowser::GetImageData()
{
	return m_ImageData;
}

MessageQueue& ChromiumBrowser::GetMessageQueue()
{
	return m_MessageQueue;
}

void ChromiumBrowser::QueueMessage( MessageQueue::Message&& message )
{
	m_MessageQueue.Lock();
	m_MessageQueue.Push( std::move( message ) );
	m_MessageQueue.Unlock();
}

//
// Main thread queues these for the UI thread
//
void ChromiumBrowser::Close()
{
	RunOrDeferForInit([this] {
		m_BrowserHost->CloseBrowser(true);
	});
}

void ChromiumBrowser::SetSize( int wide, int tall )
{
	RunOrDeferForInit([this, wide, tall] {
		m_Wide = wide;
		m_Tall = tall;
		m_BrowserHost->WasResized();
	});
}

void ChromiumBrowser::SetFocused( bool hasFocus )
{
	RunOrDeferForInit([this, hasFocus] {
		m_BrowserHost->SetFocus(hasFocus);
	});
}

void ChromiumBrowser::SendKeyEvent( IHtmlClient::KeyEvent keyEvent )
{
	if (m_BrowserHost == nullptr) {
		return;
	}

	CefKeyEvent chromiumKeyEvent;
	chromiumKeyEvent.modifiers = GetModifiers( keyEvent.modifiers );

	switch ( keyEvent.eventType )
	{
		case IHtmlClient::KeyEvent::Type::KeyChar:
			chromiumKeyEvent.type = KEYEVENT_CHAR;
			chromiumKeyEvent.character = static_cast<char16>( keyEvent.key_char );
			chromiumKeyEvent.unmodified_character = static_cast<char16>( keyEvent.key_char );
#ifdef OSX
			chromiumKeyEvent.windows_key_code = 0;
			chromiumKeyEvent.native_key_code = keyEvent.native_key_code;
#else
			chromiumKeyEvent.windows_key_code = static_cast<char16>( keyEvent.key_char );
			chromiumKeyEvent.native_key_code = static_cast<char16>( keyEvent.key_char );
#endif
			break;
		case IHtmlClient::KeyEvent::Type::KeyDown:
			chromiumKeyEvent.type = KEYEVENT_KEYDOWN;
			chromiumKeyEvent.windows_key_code = keyEvent.windows_key_code;
#ifndef _WIN32
			chromiumKeyEvent.native_key_code = keyEvent.native_key_code;
#else
			chromiumKeyEvent.native_key_code = keyEvent.windows_key_code;
#endif
			break;
		case IHtmlClient::KeyEvent::Type::KeyUp:
			chromiumKeyEvent.type = KEYEVENT_KEYUP;
			chromiumKeyEvent.windows_key_code = keyEvent.windows_key_code;
#ifndef _WIN32
			chromiumKeyEvent.native_key_code = keyEvent.native_key_code;
#else
			chromiumKeyEvent.native_key_code = keyEvent.windows_key_code;
#endif
			break;
	}

	m_BrowserHost->SendKeyEvent( chromiumKeyEvent );
}

void ChromiumBrowser::SendMouseMoveEvent( IHtmlClient::MouseEvent gmodMouseEvent, bool mouseLeave )
{
	if (m_BrowserHost == nullptr) {
		return;
	}

	CefMouseEvent mouseEvent;
	mouseEvent.x = gmodMouseEvent.x;
	mouseEvent.y = gmodMouseEvent.y;
	mouseEvent.modifiers = GetModifiers( gmodMouseEvent.modifiers );

	m_BrowserHost->SendMouseMoveEvent( mouseEvent, mouseLeave );
}

void ChromiumBrowser::SendMouseWheelEvent( IHtmlClient::MouseEvent gmodMouseEvent, int deltaX, int deltaY )
{
	if (m_BrowserHost == nullptr) {
		return;
	}

	// Some CEF bug is fucking this up. I don't care much for worrying about it yet
	CefMouseEvent mouseEvent;
	mouseEvent.x = gmodMouseEvent.x;
	mouseEvent.y = gmodMouseEvent.y;
	mouseEvent.modifiers = GetModifiers( gmodMouseEvent.modifiers );

	m_BrowserHost->SendMouseWheelEvent( mouseEvent, deltaX, deltaY );
}

void ChromiumBrowser::SendMouseClickEvent( IHtmlClient::MouseEvent gmodMouseEvent, IHtmlClient::MouseButton gmodButtonType, bool mouseUp, int clickCount )
{
	if (m_BrowserHost == nullptr) {
		return;
	}

	CefMouseEvent mouseEvent;
	mouseEvent.x = gmodMouseEvent.x;
	mouseEvent.y = gmodMouseEvent.y;
	mouseEvent.modifiers = GetModifiers( gmodMouseEvent.modifiers );

	CefBrowserHost::MouseButtonType buttonType = MBT_LEFT;

	switch ( gmodButtonType )
	{
		case IHtmlClient::MouseButton::Left:
			buttonType = MBT_LEFT;
			break;
		case IHtmlClient::MouseButton::Middle:
			buttonType = MBT_MIDDLE;
			break;
		case IHtmlClient::MouseButton::Right:
			buttonType = MBT_RIGHT;
			break;
	}

	m_BrowserHost->SendMouseClickEvent( mouseEvent, buttonType, mouseUp, clickCount );
}

void ChromiumBrowser::LoadUrl( const std::string& url )
{
	RunOrDeferForInit([this, url] {
		m_Browser->GetMainFrame()->LoadURL(CefString(url));
	});
}

void ChromiumBrowser::SetHtml( const std::string& html )
{
	RunOrDeferForInit([this, html] {
		CefURLParts urlParts;
		CefString(&urlParts.scheme).FromString("asset");
		CefString(&urlParts.host).FromString("html");
		CefString(&urlParts.path).FromString("/");
		CefString(&urlParts.query).FromString(CefBase64Encode(html.c_str(), html.size()));

		CefString url;
		if (!CefCreateURL(urlParts, url))
			return;

		m_Browser->GetMainFrame()->LoadURL(url);
	});
}

void ChromiumBrowser::Refresh()
{
	RunOrDeferForInit([this] {
		m_Browser->Reload();
	});
}

void ChromiumBrowser::Stop()
{
	RunOrDeferForInit([this] {
		m_Browser->StopLoad();
	});
}

void ChromiumBrowser::GoBack()
{
	RunOrDeferForInit([this] {
		m_Browser->GoBack();
	});
}

void ChromiumBrowser::GoForward()
{
	RunOrDeferForInit([this] {
		m_Browser->GoForward();
	});
}

void ChromiumBrowser::RunJavaScript( const std::string& code )
{
	RunOrDeferForInit([this, code] {
		auto message = CefProcessMessage::Create("ExecuteJavaScript");
		auto args = message->GetArgumentList();

		args->SetString(0, "Lua File");
		args->SetString(1, code);
		m_Browser->GetMainFrame()->SendProcessMessage(PID_RENDERER, message);
	});
}

void ChromiumBrowser::RegisterJavaScriptFunction( const std::string& objName, const std::string& funcName )
{
	RunOrDeferForInit([this, objName, funcName] {
		auto message = CefProcessMessage::Create("RegisterFunction");
		auto args = message->GetArgumentList();

		args->SetString(0, objName);
		args->SetString(1, funcName);
		m_Browser->GetMainFrame()->SendProcessMessage(PID_RENDERER, message);
	});
}

void ChromiumBrowser::SetOpenLinksExternally( bool openLinksExternally )
{
	m_OpenLinksExternally = openLinksExternally;
}

void ChromiumBrowser::ExecuteCallback( int callbackId, const JSValue& paramsArray )
{
	RunOrDeferForInit([this, callbackId, paramsArray] {
		const auto& paramsVector = (static_cast<const JSArray*>(paramsArray.GetInternalArray()))->GetInternalData(); // spaghetti
		auto message = CefProcessMessage::Create("ExecuteCallback");
		auto outArgs = message->GetArgumentList();

		auto cefArgs = CefListValue::Create();
		if (!JSValuesToCefList(cefArgs, paramsVector))
			return;

		outArgs->SetInt(0, callbackId);
		outArgs->SetList(1, cefArgs);

		m_Browser->GetMainFrame()->SendProcessMessage(PID_RENDERER, message);
	});
}

//
// CefClient interface
//
bool ChromiumBrowser::OnProcessMessageReceived( CefRefPtr<CefBrowser>, CefRefPtr<CefFrame> frame, CefProcessId, CefRefPtr<CefProcessMessage> message )
{
	auto name = message->GetName();
	auto args = message->GetArgumentList();

	if ( name != "ExecuteFunction" )
		return false;

	MessageQueue::Message msg;
	msg.type = MessageQueue::Type::OnJavaScriptCall;
	msg.string1 = args->GetString( 0 ).ToString(); // objName
	msg.string2 = args->GetString( 1 ).ToString(); // funcName
	msg.integer = args->GetInt( 2 ); // callbackId

	std::vector<JSValue> values;
	if ( !CefListToJSValues( values, args->GetList( 3 ) ) )
		return true;

	msg.jsParams = JSArray::Create( std::move( values ) );

	QueueMessage( std::move( msg ) );
	return true;
}

//
// CefLifeSpanHandler interface
//
void ChromiumBrowser::OnAfterCreated( CefRefPtr<CefBrowser> browser )
{
	m_Browser = browser;
	m_BrowserHost = browser->GetHost();

	for (auto& func : m_Deferred) {
		func();
	}

	m_Deferred = {};
}

void ChromiumBrowser::OnBeforeClose( CefRefPtr<CefBrowser> browser )
{
	m_Browser = nullptr;
	m_BrowserHost = nullptr;
}

bool ChromiumBrowser::OnBeforePopup( CefRefPtr<CefBrowser>,
	CefRefPtr<CefFrame> frame,
	const CefString& targetUrl,
	const CefString&,
	CefLifeSpanHandler::WindowOpenDisposition targetDisposition,
	bool,
	const CefPopupFeatures&,
	CefWindowInfo&,
	CefRefPtr<CefClient>&,
	CefBrowserSettings&,
	CefRefPtr<CefDictionaryValue>&,
	bool* )
{
	CefString sourceUrl = frame->GetURL();

	MessageQueue::Message msg;
	msg.type = MessageQueue::Type::OnCreateChildView;
	msg.string1 = sourceUrl.ToString();
	msg.string2 = targetUrl.ToString();
	msg.integer = static_cast<int>( targetDisposition == WOD_NEW_POPUP );
	QueueMessage( std::move( msg ) );

	// Don't create the popup
	return true;
}

//
// CefLoadHandler interface
//
void ChromiumBrowser::OnLoadStart( CefRefPtr<CefBrowser>, CefRefPtr<CefFrame> frame, CefLoadHandler::TransitionType )
{
	if ( !frame->IsMain() )
		return;

	MessageQueue::Message msg;
	msg.type = MessageQueue::Type::OnLoadStart;
	msg.string1 = frame->GetURL().ToString();
	QueueMessage( std::move( msg ) );
}

void ChromiumBrowser::OnLoadEnd( CefRefPtr<CefBrowser>, CefRefPtr<CefFrame> frame, int httpStatusCode )
{
	if ( !frame->IsMain() )
		return;

	MessageQueue::Message msg;
	msg.type = MessageQueue::Type::OnLoadEnd;
	msg.string1 = frame->GetURL().ToString();
	QueueMessage( std::move( msg ) );
}

void ChromiumBrowser::OnLoadError( CefRefPtr<CefBrowser>, CefRefPtr<CefFrame> frame, CefLoadHandler::ErrorCode errorCode, const CefString& errorText, const CefString& failedURL )
{
	if ( !frame->IsMain() )
		return;

	MessageQueue::Message msg;
	msg.type = MessageQueue::Type::OnLoadEnd;
	msg.string1 = frame->GetURL().ToString();
	QueueMessage( std::move( msg ) );
}

//
// CefDisplayHandler interface
//
void ChromiumBrowser::OnAddressChange( CefRefPtr<CefBrowser>, CefRefPtr<CefFrame> frame, const CefString& url )
{
	if ( !frame->IsMain() )
		return;

	MessageQueue::Message msg;
	msg.type = MessageQueue::Type::OnAddressChange;
	msg.string1 = url.ToString();
	QueueMessage( std::move( msg ) );
}

bool ChromiumBrowser::OnConsoleMessage( CefRefPtr<CefBrowser>, cef_log_severity_t level, const CefString& message, const CefString& source, int line )
{
	MessageQueue::Message msg;
	msg.type = MessageQueue::Type::OnConsoleMessage;
	msg.string1 = message.ToString();
	msg.string2 = source.ToString();
	msg.integer = line;
	QueueMessage( std::move( msg ) );

	return false;
}

void ChromiumBrowser::OnTitleChange( CefRefPtr<CefBrowser>, const CefString& title )
{
	MessageQueue::Message msg;
	msg.type = MessageQueue::Type::OnTitleChange;
	msg.string1 = title.ToString();
	QueueMessage( std::move( msg ) );
}

bool ChromiumBrowser::OnCursorChange( CefRefPtr<CefBrowser> browser, CefCursorHandle, cef_cursor_type_t chromeCursor, const CefCursorInfo& )
{
	using GModCursorType = IHtmlClientListener::CursorType;
	GModCursorType gmodCursor;

	switch ( chromeCursor )
	{
		case CT_POINTER:
			gmodCursor = GModCursorType::Default;
			break;
		case CT_CROSS:
			gmodCursor = GModCursorType::Crosshair;
			break;
		case CT_HAND:
			gmodCursor = GModCursorType::Hand;
			break;
		case CT_IBEAM:
			gmodCursor = GModCursorType::IBeam;
			break;
		case CT_WAIT:
			gmodCursor = GModCursorType::Hourglass;
			break;
		case CT_MOVE:
			gmodCursor = GModCursorType::SizeAll;
			break;
		case CT_EASTRESIZE:
		case CT_WESTRESIZE:
		case CT_EASTWESTRESIZE:
			gmodCursor = GModCursorType::SizeEW;
			break;
		case CT_NORTHRESIZE:
		case CT_SOUTHRESIZE:
		case CT_NORTHSOUTHRESIZE:
			gmodCursor = GModCursorType::SizeNS;
			break;
		case CT_NORTHEASTRESIZE:
		case CT_SOUTHWESTRESIZE:
		case CT_NORTHEASTSOUTHWESTRESIZE:
			gmodCursor = GModCursorType::SizeNESW;
			break;
		case CT_NORTHWESTRESIZE:
		case CT_SOUTHEASTRESIZE:
		case CT_NORTHWESTSOUTHEASTRESIZE:
			gmodCursor = GModCursorType::SizeNWSE;
			break;
		case CT_COLUMNRESIZE:
			gmodCursor = GModCursorType::SizeEW;
			break;
		case CT_ROWRESIZE:
			gmodCursor = GModCursorType::SizeNS;
			break;
		case CT_NOTALLOWED:
			gmodCursor = GModCursorType::No;
			break;
		case CT_NONE:
			gmodCursor = GModCursorType::None;
			break;
		case CT_MIDDLEPANNING:
		case CT_EASTPANNING:
		case CT_NORTHPANNING:
		case CT_NORTHEASTPANNING:
		case CT_NORTHWESTPANNING:
		case CT_SOUTHPANNING:
		case CT_SOUTHEASTPANNING:
		case CT_SOUTHWESTPANNING:
		case CT_WESTPANNING:
			gmodCursor = GModCursorType::SizeAll;
			break;
		default:
			gmodCursor = GModCursorType::Default;
	}

	MessageQueue::Message msg;
	msg.type = MessageQueue::Type::OnCursorChange;
	msg.integer = static_cast<int>( gmodCursor );
	QueueMessage( std::move( msg ) );

	return false;
}

//
// CefRenderHandler interface
//
void ChromiumBrowser::GetViewRect( CefRefPtr<CefBrowser>, CefRect& rect )
{
	rect.x = 0;
	rect.y = 0;
	rect.width = m_Wide;
	rect.height = m_Tall;
}


void ChromiumBrowser::OnPopupShow( CefRefPtr<CefBrowser>, bool show )
{
	if ( !show )
	{
		m_PopupX = 0;
		m_PopupY = 0;
		m_PopupWide = 0;
		m_PopupTall = 0;

		if ( m_PopupData )
		{
			delete[] m_PopupData;
			m_PopupData = nullptr;
		}

		// Re-draw the view without our popup
		m_BrowserHost->Invalidate( PET_VIEW );
	}
}
void ChromiumBrowser::OnPopupSize( CefRefPtr<CefBrowser>, const CefRect& rect )
{
	// We can grab the width and height in OnPaint, but these two are only available here >:[
	m_PopupX = rect.x;
	m_PopupY = rect.y;
}

bool ShouldFullCopy( const CefRenderHandler::RectList& dirtyRects, int width, int height ) {
	int dirty_area = 0;
	for (auto &&rect : dirtyRects) {
		dirty_area += rect.width * rect.height;
	}
	// TODO: Find optimal threshold using benchmarking.
	return dirty_area > 0.8 * width * height;
}

void ChromiumBrowser::OnPaint( CefRefPtr<CefBrowser>, CefRenderHandler::PaintElementType type, const CefRenderHandler::RectList& dirtyRects, const void* buffer, int width, int height )
{
	//
	// We blit the popup straight on to the main image. That means gmod won't need to use multiple textures (+1)
	//
	switch ( type )
	{
		case PET_POPUP:
			m_PopupWide = width;
			m_PopupTall = height;
			m_PopupData = new unsigned char[width * height * 4];
			memcpy( m_PopupData, buffer, width * height * 4 );

			// Re-draw the view with our popup
			m_BrowserHost->Invalidate( PET_VIEW );
			return;

		case PET_VIEW:
			m_ImageData.Lock();

			if ( m_ImageData.ResizeData( width, height ) || ShouldFullCopy( dirtyRects, width, height ) ) {
				// Copy whole buffer over ImageData
				m_ImageData.SetData(static_cast<const unsigned char*>(buffer), width, height);
			} else {
				// Blit the dirty parts of buffer over ImageData
				for (auto &&rect : dirtyRects)
				{
					if (!rect.IsEmpty()) {
						m_ImageData.Blit(static_cast<const unsigned char*>(buffer), rect.x, rect.y, rect.width, rect.height);
					}
				}
			}

			// Blit our popup over the ImageData
			if ( m_PopupWide > 0 && m_PopupTall > 0 )
			{
				m_ImageData.BlitRelative(m_PopupData, m_PopupX, m_PopupY, m_PopupWide, m_PopupTall);
			}

			m_ImageData.Unlock();
			return;
	}
}

void ChromiumBrowser::OnAcceleratedPaint(CefRefPtr<CefBrowser> browser, CefRenderHandler::PaintElementType type, const CefRenderHandler::RectList& dirtyRects, void* shared_handle)
{
	// TODO: Implement once fixed for OSR on Viz
	// TODO: See ChromiumSystem::CreateClient
	// https://bitbucket.org/chromiumembedded/cef/pull-requests/285/reimplement-shared-texture-support-for-viz

	LOG(ERROR) << "ChromiumBrowser::OnAcceleratedPaint";
}

//
// CefRequestHandler interface
//
bool ChromiumBrowser::OnBeforeBrowse( CefRefPtr<CefBrowser>,
	CefRefPtr<CefFrame>,
	CefRefPtr<CefRequest> request,
	bool,
	bool )
{
	CefURLParts urlParts;
	if ( !CefParseURL( request->GetURL(), urlParts ) )
		return true;

	CefString strScheme	( &urlParts.scheme );

	// Let us see the credits :)
	if ( request->GetURL() == "chrome://credits/" )
	{
		return false;
	}

	if ( strScheme != "http" && strScheme != "https" && strScheme != "asset" )
	{
		return true;
	}

	// TODO: Remove all of this
#ifndef __APPLE__
	if ( m_OpenLinksExternally )
	{
#if defined( _WIN32 )
		ShellExecute( NULL, "open", request->GetURL().ToString().c_str(), NULL, NULL, SW_SHOWNORMAL );
#elif defined( __linux__ )
		std::string strUrl = request->GetURL().ToString();
		pid_t pid;
		const char* args[3];
		args[0] = "/usr/bin/xdg-open";
		args[1] = strUrl.c_str();
		args[2] = NULL;
		pid = fork();

		if ( pid == 0 )
		{
			execvp( args[0], (char* const*) args );
		}
#else
#error
#endif
		return true;
	}
#endif

	return false;
}

ChromiumBrowser::ReturnValue ChromiumBrowser::OnBeforeResourceLoad( CefRefPtr<CefBrowser>,
	CefRefPtr<CefFrame>,
	CefRefPtr<CefRequest> request,
	CefRefPtr<CefCallback> )
{
	CefURLParts urlParts;
	if ( !CefParseURL( request->GetURL(), urlParts ) )
		return RV_CANCEL;

	CefString strScheme	( &urlParts.scheme );

	// Let us see the credits :)
	if ( request->GetURL() == "chrome://credits/" ||
		request->GetURL() == "chrome://resources/js/cr.js" ||
		request->GetURL() == "chrome://credits/credits.js" )
	{
		return RV_CONTINUE;
	}

	if ( strScheme != "http" && strScheme != "https" && strScheme != "asset" )
	{
		return RV_CANCEL;
	}

	return RV_CONTINUE;
}

void ChromiumBrowser::OnProtocolExecution( CefRefPtr<CefBrowser>,
	CefRefPtr<CefFrame>,
	CefRefPtr<CefRequest>,
	bool& allow_os_execution )
{
	allow_os_execution = false;
}


//
// CefContextMenuHandler interface
//
void ChromiumBrowser::OnBeforeContextMenu( CefRefPtr<CefBrowser>,
	CefRefPtr<CefFrame>,
	CefRefPtr<CefContextMenuParams>,
	CefRefPtr<CefMenuModel> model )
{
	model->Clear();
}

//
// CefDialogHandler interface
//
bool ChromiumBrowser::OnFileDialog( CefRefPtr<CefBrowser>,
	FileDialogMode,
	const CefString&,
	const CefString&,
	const std::vector<CefString>&,
	int,
	CefRefPtr<CefFileDialogCallback> callback )
{
	callback->Cancel();
	return true;
}

//
// CefJSDialogHandler
//
bool ChromiumBrowser::OnJSDialog( CefRefPtr<CefBrowser>,
	const CefString&,
	JSDialogType,
	const CefString&,
	const CefString&,
	CefRefPtr<CefJSDialogCallback>,
	bool& suppress_message )
{
	suppress_message = true;
	return false;
}

bool ChromiumBrowser::OnBeforeUnloadDialog( CefRefPtr<CefBrowser>,
	const CefString&,
	bool,
	CefRefPtr<CefJSDialogCallback> callback )
{
	callback->Continue( true, CefString() );
	return true;
}
