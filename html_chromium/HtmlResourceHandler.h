#pragma once

#include "cef_start.h"
#include "include/cef_base.h"
#include "include/cef_resource_handler.h"
#include "include/cef_scheme.h"
#include "cef_end.h"

#include <vector>

class HtmlResourceHandlerFactory
	: public CefSchemeHandlerFactory
{
public:
	CefRefPtr<CefResourceHandler> Create( CefRefPtr<CefBrowser>, CefRefPtr<CefFrame>, const CefString&, CefRefPtr<CefRequest> request ) override;

private:
	IMPLEMENT_REFCOUNTING( HtmlResourceHandlerFactory );
};

//

class HtmlResourceHandler
	: public CefResourceHandler
{
public:
	//
	// CefResourceHandler interface
	//
	bool ProcessRequest( CefRefPtr<CefRequest> request, CefRefPtr<CefCallback> callback ) override;
	void GetResponseHeaders( CefRefPtr<CefResponse> response, int64_t& responseLength, CefString& redirectUrl ) override;
	bool ReadResponse( void* data_out, int bytes_to_read, int& bytes_read, CefRefPtr<CefCallback> callback ) override;
	void Cancel() override;

private:
	std::vector<char> m_Html;
	int m_Current;

private:
	IMPLEMENT_REFCOUNTING( HtmlResourceHandler );
};
