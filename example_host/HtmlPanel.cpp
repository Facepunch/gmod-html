
#include <iostream>

#include "HtmlPanel.h"
#include "HtmlSystemLoader.h"
#include "glad/glad.h"

HtmlPanel::HtmlPanel()
	: m_Texture( nullptr )
	, m_TextureWidth( -1 )
	, m_TextureHeight( -1 )
{
	m_HtmlClient = g_pHtmlSystem->CreateClient( this );
}

HtmlPanel::~HtmlPanel()
{
	m_HtmlClient->Close();
	m_HtmlClient = nullptr;

	DestroyTexture();
}

void HtmlPanel::LoadUrl( const char* pUrl )
{
	m_HtmlClient->LoadUrl( pUrl );
}

void HtmlPanel::UpdateTexture()
{
	int width, height;
	const unsigned char* data;

	if ( !m_HtmlClient->LockImageData() )
		return;

	data = m_HtmlClient->GetImageData( width, height );

	if ( m_Texture == nullptr || m_TextureWidth != width || m_TextureHeight != height )
	{
		// Make a new texture (technically an unnecessary destroy but it looks like this in Source)
		DestroyTexture();

		// No error handling?
		GLuint texture;
		glGenTextures( 1, &texture );
		glBindTexture( GL_TEXTURE_2D, texture );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_BGRA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, data );

		m_Texture = reinterpret_cast<ImTextureID>( static_cast<uintptr_t>( texture ) );
		m_TextureWidth = width;
		m_TextureHeight = height;
	}
	else
	{
		// Update current texture
		glBindTexture( GL_TEXTURE_2D, static_cast<GLuint>( reinterpret_cast<uintptr_t>( m_Texture ) ) );
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, data );
	}

	m_HtmlClient->UnlockImageData();
}

void HtmlPanel::DestroyTexture()
{
	m_Texture = nullptr;
	m_TextureWidth = -1;
	m_TextureHeight = -1;
}

void HtmlPanel::Render()
{
	UpdateTexture();

	if ( m_Texture == nullptr )
		return;

	if ( ImGui::Begin( "Browser", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar ) )
	{
		ImGui::Image( m_Texture, ImVec2( m_TextureWidth, m_TextureHeight ) );
	}

	ImGui::End();
}

void HtmlPanel::OnAddressChange( const char* address )
{
	std::cout << "OnAddressChange: " << address << std::endl;
}

void HtmlPanel::OnConsoleMessage( const char* message, const char* source, int lineNumber )
{
	std::cout << source << ":" << lineNumber << ": " << message << std::endl;
}

void HtmlPanel::OnTitleChange( const char* title )
{
	std::cout << "OnTitleChange: " << title << std::endl;
}

void HtmlPanel::OnTargetUrlChange( const char* url )
{
	std::cout << "OnTargetUrlChange: " << url << std::endl;
}

void HtmlPanel::OnCursorChange( CursorType cursorType )
{

}

void HtmlPanel::OnLoadStart( const char* address )
{
	std::cout << "OnLoadStart: " << address << std::endl;
}

void HtmlPanel::OnLoadEnd( const char* address )
{
	std::cout << "OnLoadEnd: " << address << std::endl;
}

void HtmlPanel::OnDocumentReady( const char* address )
{
	std::cout << "OnDocumentReady: " << address << std::endl;
}

void HtmlPanel::OnCreateChildView( const char* sourceUrl, const char* targetUrl, bool isPopup )
{

}

JSValue HtmlPanel::OnJavaScriptCall( const char* objName, const char* funcName, const JSValue& params )
{
	return JSValue();
}
