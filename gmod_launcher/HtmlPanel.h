#pragma once

#include "html/IHtmlSystem.h"
#include "imgui.h"

class HtmlPanel : public IHtmlClientListener
{
public:
	HtmlPanel();
	~HtmlPanel();

	HtmlPanel( const HtmlPanel& ) = delete;
	HtmlPanel( HtmlPanel&& ) = delete;

	HtmlPanel& operator=( const HtmlPanel& ) = delete;
	HtmlPanel& operator=( HtmlPanel&& ) = delete;

	void LoadUrl( const char* pUrl );

	void Render();

	//
	// IHtmlClientListener
	//
	void OnAddressChange( const char* address ) override;
	void OnConsoleMessage( const char* message, const char* source, int lineNumber ) override;
	void OnTitleChange( const char* title ) override;
	void OnTargetUrlChange( const char* url ) override;
	void OnCursorChange( CursorType cursorType ) override;
	void OnLoadStart( const char* address ) override;
	void OnLoadEnd( const char* address ) override;
	void OnDocumentReady( const char* address ) override;
	void OnCreateChildView( const char* sourceUrl, const char* targetUrl, bool isPopup ) override;
	JSValue OnJavaScriptCall( const char* objName, const char* funcName, const JSValue& params ) override;

private:
	void UpdateTexture();
	void DestroyTexture();

	IHtmlClient* m_HtmlClient;

	ImTextureID m_Texture;
	int m_TextureWidth;
	int m_TextureHeight;
};