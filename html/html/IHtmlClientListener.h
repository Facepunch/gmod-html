#pragma once

#include "JSValue.h"

//
// This is implemented by game code and handles events coming from the UI thread
// These methods will be called on the main thread, either from IHtmlSystem::Update or any of IHtmlClient's methods
//
class IHtmlClientListener
{
public:
	enum class CursorType
	{
		Default,
		Crosshair,
		Hand,
		IBeam,
		Hourglass,
		SizeEW,
		SizeNS,
		SizeNWSE,
		SizeNESW,
		SizeAll,
		No,
		None,
	};

public:
	virtual void OnAddressChange( const char* address ) = 0;
	virtual void OnConsoleMessage( const char* message, const char* source, int lineNumber ) = 0;
	virtual void OnTitleChange( const char* title ) = 0;
	virtual void OnTargetUrlChange( const char* url ) = 0;
	virtual void OnCursorChange( CursorType cursorType ) = 0;

	virtual void OnLoadStart( const char* address ) = 0;
	virtual void OnLoadEnd( const char* address ) = 0;
	virtual void OnDocumentReady( const char* address ) = 0;

	virtual void OnCreateChildView( const char* sourceUrl, const char* targetUrl, bool isPopup ) = 0;

	// The input and output JSValue instances should be arrays!!!
	virtual JSValue OnJavaScriptCall( const char* objName, const char* funcName, const JSValue& params ) = 0;
};
