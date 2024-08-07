#pragma once

//
// The IHtmlClient allows for sending data from the game's main thread to the browser's UI thread
// All of these should be called from the main thread
//
class IHtmlClient
{
public:
	// Additional modifiers passed with key or mouse events
	enum class EventModifiers : int
	{
		None = 0,
		Shift = (1 << 0),
		Control = (1 << 1),
		Alt = (1 << 2),
		LeftMouse = (1 << 3),
		MiddleMouse = (1 << 4),
		RightMouse = (1 << 5),
		OSX_Cmd = (1 << 6),
	};

	struct KeyEvent
	{
		enum class Type
		{
			KeyDown,
			KeyUp,
			KeyChar,
		};

		Type eventType;

		union
		{
			unsigned short key_char; // Type::KeyChar
			int windows_key_code;    // Type::KeyDown / Type::KeyUp
		};

		// TODO: Fix this Facepunch! See ChromiumBrowser::SendKeyEvent
#ifndef _WIN32
		int native_key_code;
#endif

		EventModifiers modifiers;
	};

	enum class MouseButton
	{
		Left,
		Middle,
		Right,
	};

	struct MouseEvent
	{
		int x;
		int y;
		EventModifiers modifiers;
	};

public:
	// Closes the client. You should treat the IHtmlClient as destroyed after calling this.
	virtual void Close() = 0;

	virtual void SetSize( int wide, int tall ) = 0;
	virtual void SetFocused( bool hasFocus ) = 0;

	virtual void SendKeyEvent( KeyEvent keyEvent ) = 0;

	virtual void SendMouseMoveEvent( MouseEvent mouseEvent, bool mouseLeave ) = 0;
	virtual void SendMouseWheelEvent( MouseEvent mouseEvent, int deltaX, int deltaY ) = 0;
	virtual void SendMouseClickEvent( MouseEvent mouseEvent, MouseButton mouseButton, bool mouseUp, int clickCount ) = 0;

	// Navigation calls.
	virtual void LoadUrl( const char* url ) = 0;
	virtual void SetHtml( const char* html ) = 0;
	virtual void Refresh() = 0;
	virtual void Stop() = 0;
	virtual void GoBack() = 0;
	virtual void GoForward() = 0;

	// Simple executes some JavaScript. No queueing, let Lua worry about that.
	virtual void RunJavaScript( const char* code ) = 0;

	// Creates a JavaScript function at <objName>.<funcName>. Any calls to that function go to this page's JS call handler.
	virtual void RegisterJavaScriptFunction( const char* objName, const char* funcName ) = 0;

	// Opens clicekd links in the user's browser. This should only be set on clients that we trust.
	virtual void SetOpenLinksExternally( bool openLinksExternally ) = 0;

	//
	// Access to the client's image data. Make sure to lock/unlock around these methods
	// If lock returns false, just act as if there's nothing new to paint
	//
	virtual bool LockImageData() = 0;
	virtual void UnlockImageData() = 0;
	virtual const unsigned char* GetImageData( int& imageWide, int& imageTall ) = 0;
};
