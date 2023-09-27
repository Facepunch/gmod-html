#include "ResourceHandler.h"
#include "ChromiumSystem.h"

#include "cef_start.h"
#include "include/cef_parser.h"
#include "cef_end.h"

IHtmlResourceHandler* g_pHtmlResourceHandler = nullptr;

CefRefPtr<CefResourceHandler> ResourceHandlerFactory::Create( CefRefPtr<CefBrowser>, CefRefPtr<CefFrame>, const CefString&, CefRefPtr<CefRequest> request )
{
	return new ResourceHandler();
}

//

ResourceHandler::ResourceHandler()
	: m_Buffer( nullptr )
	, m_BufferPtr( nullptr )
	, m_BufferLength( 0 )
{}

ResourceHandler::~ResourceHandler()
{
	// The code does try to clean-up early, but if that fails this will take care of it.
	delete[] m_Buffer;
	m_Buffer = nullptr;
}

void ResourceHandler::Handle()
{
	HtmlResource* pResource = g_pHtmlResourceHandler->OpenResource( m_Host.c_str(), m_Path.c_str() );

	if ( pResource != nullptr )
	{
		size_t size = g_pHtmlResourceHandler->GetLength( pResource );
		m_Buffer = new char[size];
		m_BufferPtr = m_Buffer;
		m_BufferLength = size;

		g_pHtmlResourceHandler->ReadData( pResource, m_Buffer, size );
		g_pHtmlResourceHandler->CloseResource( pResource );
	}

	// Now we can allow the IO thread to continue the response
	m_ResponseCallback->Continue();
}

//
// CefResourceHandler interface
//
bool ResourceHandler::ProcessRequest( CefRefPtr<CefRequest> request, CefRefPtr<CefCallback> callback )
{
	CefURLParts urlParts;
	if ( !CefParseURL( request->GetURL(), urlParts ) )
		return false;

	auto pathEncoded = CefString( &urlParts.path );
	std::string pathEncodedStr = pathEncoded.ToString();

	// Path-traversal mitigation
	// TODO: This should really be implemented in GMod itself!
	if (pathEncodedStr[2] == ':' || pathEncodedStr.rfind("//", 0) == 0) {
		return false;
	}

	m_Host = CefString( &urlParts.host ).ToString();
	m_Path = CefURIDecode( pathEncoded, true, (cef_uri_unescape_rule_t) ( UU_SPACES | UU_URL_SPECIAL_CHARS_EXCEPT_PATH_SEPARATORS ) ).ToString();

	m_ResponseCallback = callback;
	g_ChromiumSystem.QueueRequest( this );

	return true;
}

static std::string GetFileExtension( std::string& path )
{
	size_t i = path.find_last_of( '.' );
	if ( i == std::string::npos ) { return ""; }
	return path.substr( i + 1, path.length() );
}

void ResourceHandler::GetResponseHeaders( CefRefPtr<CefResponse> response, int64_t& responseLength, CefString& redirectUrl )
{
	if ( m_Buffer == nullptr )
	{
		responseLength = 0;
		response->SetError( ERR_FILE_NOT_FOUND );
		return;
	}

	CefString mimeType = "text/html";

	auto ext = GetFileExtension( m_Path );
	if ( ext == "png" ) mimeType = "image/png";
	else if ( ext == "jpg" ) mimeType = "image/jpg";
	else if ( ext == "jpeg" ) mimeType = "image/jpg";
	else if ( ext == "css" ) mimeType = "text/css";
	else if ( ext == "js" ) mimeType = "text/javascript";

	response->SetStatus( 200 );
	response->SetMimeType( mimeType );
	responseLength = m_BufferLength;
}

bool ResourceHandler::ReadResponse( void* data_out, int bytes_to_read, int& bytes_read, CefRefPtr<CefCallback> callback )
{
	bytes_to_read = std::min<uint64_t>( bytes_to_read, m_BufferLength - ( m_BufferPtr - m_Buffer ) );

	memcpy( data_out, m_BufferPtr, bytes_to_read );
	bytes_read = bytes_to_read;
	m_BufferPtr += bytes_to_read;

	if ( bytes_to_read == 0 )
	{
		// We're done. Clean up this buffer early just to save on memory for a few moments.
		delete[] m_Buffer;
		m_Buffer = nullptr;
	}

	return bytes_to_read > 0;
}

void ResourceHandler::Cancel()
{
	// ...
}
