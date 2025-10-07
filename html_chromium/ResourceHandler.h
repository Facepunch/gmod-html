#pragma once

#include "cef_start.h"
#include "include/cef_base.h"
#include "include/cef_resource_handler.h"
#include "include/cef_scheme.h"
#include "cef_end.h"

extern class IHtmlResourceHandler* g_pHtmlResourceHandler;

class ResourceHandlerFactory
	: public CefSchemeHandlerFactory
{
public:
	CefRefPtr<CefResourceHandler> Create( CefRefPtr<CefBrowser>, CefRefPtr<CefFrame>, const CefString&, CefRefPtr<CefRequest> request ) override;

private:
	IMPLEMENT_REFCOUNTING( ResourceHandlerFactory );
};

//

class ResourceHandler
	: public CefResourceHandler
{
public:
	ResourceHandler();
	~ResourceHandler();

	// This is ran from the main thread. Here, we can read the file and continue our response in the IO thread.
	void Handle();

	//
	// CefResourceHandler interface
	//
	bool ProcessRequest( CefRefPtr<CefRequest> request, CefRefPtr<CefCallback> callback ) override;
	void GetResponseHeaders( CefRefPtr<CefResponse> response, cef_int64& responseLength, CefString& redirectUrl ) override;
	bool ReadResponse( void* data_out, int bytes_to_read, int& bytes_read, CefRefPtr<CefCallback> callback ) override;
	void Cancel() override;

private:
	std::string m_Host;
	std::string m_Path;
	CefRefPtr<CefCallback> m_ResponseCallback;

	char* m_Buffer;
	const char* m_BufferPtr;
	int64_t m_BufferLength;

private:
	IMPLEMENT_REFCOUNTING( ResourceHandler );
};
