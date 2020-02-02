#include "ChromiumClient.h"
#include "ChromiumBrowser.h"
#include "ChromiumSystem.h"
#include "JSObjects.h"

#include "cef_start.h"
#include "include/base/cef_bind.h"
#include "include/wrapper/cef_closure_task.h"
#include "cef_end.h"

#include <string>

ChromiumClient::ChromiumClient( CefRefPtr<ChromiumBrowser> browser, IHtmlClientListener* listener )
	: m_Browser( browser )
	, m_Listener( listener )
{
	g_ChromiumSystem.OnClientOpen( this );
}

void ChromiumClient::Update()
{
	MessageQueue& messageQueue = m_Browser->GetMessageQueue();

	messageQueue.Lock();
	{
		while ( !messageQueue.Empty() )
		{
			auto& msg = messageQueue.Front();

			switch ( msg.type )
			{
				case MessageQueue::Type::OnAddressChange:
					m_Listener->OnAddressChange( msg.string1.c_str() );
					break;
				case MessageQueue::Type::OnConsoleMessage:
					m_Listener->OnConsoleMessage( msg.string1.c_str(), msg.string2.c_str(), msg.integer );
					break;
				case MessageQueue::Type::OnTitleChange:
					m_Listener->OnTitleChange( msg.string1.c_str() );
					break;
				case MessageQueue::Type::OnCursorChange:
					m_Listener->OnCursorChange( static_cast<IHtmlClientListener::CursorType>( msg.integer ) );
					break;
				case MessageQueue::Type::OnLoadStart:
					m_Listener->OnLoadStart( msg.string1.c_str() );
					break;
				case MessageQueue::Type::OnLoadEnd:
					m_Listener->OnDocumentReady( msg.string1.c_str() );
					m_Listener->OnLoadEnd( msg.string1.c_str() );
					break;
				case MessageQueue::Type::OnCreateChildView:
					m_Listener->OnCreateChildView( msg.string1.c_str(), msg.string2.c_str(), static_cast<bool>( msg.integer ) );
					break;
				case MessageQueue::Type::OnJavaScriptCall:
					HandleJavaScriptCall( msg.string1.c_str(), msg.string2.c_str(), msg.jsParams, msg.integer );
					break;
			}

			messageQueue.Pop();
		}
	}
	messageQueue.Unlock();
}

void ChromiumClient::HandleJavaScriptCall( const char* objName, const char* funcName, const JSValue& paramArray, int callbackId )
{
	JSValue retArray = m_Listener->OnJavaScriptCall( objName, funcName, paramArray );

	if ( callbackId == -1 )
		return;

	// TODO: This seems to be copying retArray
	CefPostTask( TID_UI, base::Bind( &ChromiumBrowser::ExecuteCallback, m_Browser, callbackId, std::move( retArray ) ) );
}

//
// IHtmlClient interface
//
void ChromiumClient::Close()
{
	CefPostTask( TID_UI, base::Bind( &ChromiumBrowser::Close, m_Browser ) );

	g_ChromiumSystem.OnClientClose( this );
	delete this;
}

void ChromiumClient::SetSize( int wide, int tall )
{
	CefPostTask( TID_UI, base::Bind( &ChromiumBrowser::SetSize, m_Browser, wide, tall ) );
}

void ChromiumClient::SetFocused( bool hasFocus )
{
	CefPostTask( TID_UI, base::Bind( &ChromiumBrowser::SetFocused, m_Browser, hasFocus ) );
}

void ChromiumClient::SendKeyEvent( KeyEvent keyEvent )
{
	CefPostTask( TID_UI, base::Bind( &ChromiumBrowser::SendKeyEvent, m_Browser, keyEvent ) );
}

void ChromiumClient::SendMouseMoveEvent( MouseEvent mouseEvent, bool mouseLeave )
{
	CefPostTask( TID_UI, base::Bind( &ChromiumBrowser::SendMouseMoveEvent, m_Browser, mouseEvent, mouseLeave ) );
}

void ChromiumClient::SendMouseWheelEvent( MouseEvent mouseEvent, int deltaX, int deltaY )
{
	CefPostTask( TID_UI, base::Bind( &ChromiumBrowser::SendMouseWheelEvent, m_Browser, mouseEvent, deltaX, deltaY ) );
}

void ChromiumClient::SendMouseClickEvent( MouseEvent mouseEvent, MouseButton mouseButton, bool mouseUp, int clickCount )
{
	CefPostTask( TID_UI, base::Bind( &ChromiumBrowser::SendMouseClickEvent, m_Browser, mouseEvent, mouseButton, mouseUp, clickCount ) );
}

void ChromiumClient::LoadUrl( const char* url )
{
	CefPostTask( TID_UI, base::Bind( &ChromiumBrowser::LoadUrl, m_Browser, std::string( url ) ) );
}

void ChromiumClient::SetHtml( const char* html )
{
	CefPostTask( TID_UI, base::Bind( &ChromiumBrowser::SetHtml, m_Browser, std::string( html ) ) );
}

void ChromiumClient::Refresh()
{
	CefPostTask( TID_UI, base::Bind( &ChromiumBrowser::Refresh, m_Browser ) );
}

void ChromiumClient::Stop()
{
	CefPostTask( TID_UI, base::Bind( &ChromiumBrowser::Stop, m_Browser ) );
}

void ChromiumClient::GoBack()
{
	CefPostTask( TID_UI, base::Bind( &ChromiumBrowser::GoBack, m_Browser ) );
}

void ChromiumClient::GoForward()
{
	CefPostTask( TID_UI, base::Bind( &ChromiumBrowser::GoForward, m_Browser ) );
}

void ChromiumClient::RunJavaScript( const char* code )
{
	CefPostTask( TID_UI, base::Bind( &ChromiumBrowser::RunJavaScript, m_Browser, std::string( code ) ) );
}

void ChromiumClient::RegisterJavaScriptFunction( const char* objName, const char* funcName )
{
	CefPostTask( TID_UI, base::Bind( &ChromiumBrowser::RegisterJavaScriptFunction, m_Browser, std::string( objName ), std::string( funcName ) ) );
}

void ChromiumClient::SetOpenLinksExternally( bool openLinksExternally )
{
	CefPostTask( TID_UI, base::Bind( &ChromiumBrowser::SetOpenLinksExternally, m_Browser, openLinksExternally ) );
}

bool ChromiumClient::LockImageData()
{
	ImageData& imageData = m_Browser->GetImageData();

	imageData.Lock();

	if ( !imageData.IsDirty() )
	{
		imageData.Unlock();
		return false;
	}

	return true;
}

void ChromiumClient::UnlockImageData()
{
	ImageData& imageData = m_Browser->GetImageData();
	imageData.SetDirty( false );
	imageData.Unlock();
}

const unsigned char* ChromiumClient::GetImageData( int& imageWide, int& imageTall )
{
	ImageData& imageData = m_Browser->GetImageData();
	return imageData.GetData( imageWide, imageTall );
}

