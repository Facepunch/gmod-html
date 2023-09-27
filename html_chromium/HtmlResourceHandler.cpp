#include "HtmlResourceHandler.h"

#include "cef_start.h"
#include "include/cef_parser.h"
#include "cef_end.h"

CefRefPtr<CefResourceHandler> HtmlResourceHandlerFactory::Create( CefRefPtr<CefBrowser>, CefRefPtr<CefFrame>, const CefString&, CefRefPtr<CefRequest> request )
{
	return new HtmlResourceHandler();
}

//
// CefResourceHandler interface
//
bool HtmlResourceHandler::ProcessRequest( CefRefPtr<CefRequest> request, CefRefPtr<CefCallback> callback )
{
	m_Current = 0;

	CefURLParts urlParts;
	if ( !CefParseURL( request->GetURL(), urlParts ) )
		return false;

	CefString encodedHtml( urlParts.query.str, urlParts.query.length, false );
	auto data = CefBase64Decode( encodedHtml );

	if ( data && data->IsValid() )
	{
		m_Html.resize( data->GetSize() );
		data->GetData( (void*) m_Html.data(), m_Html.size(), 0 );
	}

	callback->Continue();
	return true;
}

void HtmlResourceHandler::GetResponseHeaders( CefRefPtr<CefResponse> response, int64_t& responseLength, CefString& redirectUrl )
{
	CefResponse::HeaderMap headerMap;
	headerMap.emplace( "X-XSS-Protection", "0" );

	response->SetStatus( 200 );
	response->SetHeaderMap( headerMap );
	response->SetMimeType( "text/html" );
	responseLength = m_Html.size();
}

bool HtmlResourceHandler::ReadResponse( void* data_out, int bytes_to_read, int& bytes_read, CefRefPtr<CefCallback> callback )
{
	if ( m_Html.size() <= 0 )
		return false;

	bytes_to_read = std::min<uint64_t>( bytes_to_read, m_Html.size() - m_Current );

	memcpy( data_out, m_Html.data() + m_Current, bytes_to_read );
	bytes_read = bytes_to_read;
	m_Current += bytes_to_read;

	return bytes_to_read > 0;
}

void HtmlResourceHandler::Cancel()
{
	// ...
}
