#pragma once

#include "html/IHtmlSystem.h"

#include "cef_start.h"
#include "include/cef_base.h"
#include "cef_end.h"

class ChromiumBrowser;

//
// This class just handles threaded communication to/from the ChromiumBrowser instance
//
class ChromiumClient
	: public IHtmlClient
{
public:
	ChromiumClient( CefRefPtr<ChromiumBrowser> browser, IHtmlClientListener* listener );
	void Update();
	void HandleJavaScriptCall( const char* objName, const char* funcName, const JSValue& paramArray, int callbackId );

private:
	CefRefPtr<ChromiumBrowser> m_Browser;
	IHtmlClientListener* m_Listener;

public:
	//
	// IHtmlClient interface
	//
	void Close() override;
	void SetSize( int wide, int tall ) override;
	void SetFocused( bool hasFocus ) override;
	void SendKeyEvent( KeyEvent keyEvent ) override;
	void SendMouseMoveEvent( MouseEvent mouseEvent, bool mouseLeave ) override;
	void SendMouseWheelEvent( MouseEvent mouseEvent, int deltaX, int deltaY ) override;
	void SendMouseClickEvent( MouseEvent mouseEvent, MouseButton mouseButton, bool mouseUp, int clickCount ) override;
	void LoadUrl( const char* url ) override;
	void SetHtml( const char* html ) override;
	void Refresh() override;
	void Stop() override;
	void GoBack() override;
	void GoForward() override;
	void RunJavaScript( const char* code ) override;
	void RegisterJavaScriptFunction( const char* objName, const char* funcName ) override;
	void SetOpenLinksExternally( bool openLinksExternally ) override;
	bool LockImageData() override;
	void UnlockImageData() override;
	const unsigned char* GetImageData( int& imageWide, int& imageTall ) override;
};
