#pragma once

#include <stddef.h>

// Defined in game code
struct HtmlResource;

//
// This is implemented by game code and handles requests sent to asset://{x}/{y}
// Everything here should be called from the main thread.
//
class IHtmlResourceHandler
{
public:
	virtual HtmlResource* OpenResource( const char* pHost, const char* pPath ) = 0;
	virtual void CloseResource( HtmlResource* resource ) = 0;

	virtual size_t GetLength( HtmlResource* resource ) = 0;
	virtual void ReadData( HtmlResource* resource, char* pDestination, size_t length ) = 0;

	// This doesn't belong here at all
	virtual void Message( const char* data ) = 0;
};