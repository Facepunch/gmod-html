#pragma once

#include "html/IHtmlSystem.h"

class StubClient : public IHtmlClient
{
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
