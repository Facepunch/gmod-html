#include "StubClient.h"

void StubClient::Close()
{

}

void StubClient::SetSize( int wide, int tall )
{

}

void StubClient::SetFocused( bool hasFocus )
{

}

void StubClient::SendKeyEvent( KeyEvent keyEvent )
{

}

void StubClient::SendMouseMoveEvent( MouseEvent mouseEvent, bool mouseLeave )
{

}

void StubClient::SendMouseWheelEvent( MouseEvent mouseEvent, int deltaX, int deltaY )
{

}

void StubClient::SendMouseClickEvent( MouseEvent mouseEvent, MouseButton mouseButton, bool mouseUp, int clickCount )
{

}

void StubClient::LoadUrl( const char* url )
{

}

void StubClient::SetHtml( const char* html )
{

}

void StubClient::Refresh()
{

}

void StubClient::Stop()
{

}

void StubClient::GoBack()
{

}

void StubClient::GoForward()
{

}

void StubClient::RunJavaScript( const char* code )
{

}

void StubClient::RegisterJavaScriptFunction( const char* objName, const char* funcName )
{

}

void StubClient::SetOpenLinksExternally( bool openLinksExternally )
{

}

bool StubClient::LockImageData()
{
	return false;
}

void StubClient::UnlockImageData()
{

}

const unsigned char* StubClient::GetImageData( int& imageWide, int& imageTall )
{
	return nullptr;
}
