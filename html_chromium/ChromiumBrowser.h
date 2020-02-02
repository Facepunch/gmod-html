#pragma once

#include <vector>

#include "cef_start.h"
#include "include/cef_client.h"
#include "cef_end.h"

#include "MessageQueue.h"
#include "ImageData.h"

#include "html/IHtmlClient.h"

class ChromiumBrowser
	: public CefClient
	, public CefLifeSpanHandler
	, public CefLoadHandler
	, public CefDisplayHandler
	, public CefRenderHandler
	, public CefRequestHandler
	, public CefResourceRequestHandler
	, public CefContextMenuHandler
	, public CefDialogHandler
	, public CefJSDialogHandler
{
public:
	ChromiumBrowser();
	~ChromiumBrowser();

	ImageData& GetImageData();
	MessageQueue& GetMessageQueue();
	void QueueMessage( MessageQueue::Message&& message );

	//
	// Main thread calls these
	//
	void Close();
	void SetSize( int wide, int tall );
	void SetFocused( bool hasFocus );
	void SendKeyEvent( IHtmlClient::KeyEvent keyEvent );
	void SendMouseMoveEvent( IHtmlClient::MouseEvent mouseEvent, bool mouseLeave );
	void SendMouseWheelEvent( IHtmlClient::MouseEvent mouseEvent, int deltaX, int deltaY );
	void SendMouseClickEvent( IHtmlClient::MouseEvent mouseEvent, IHtmlClient::MouseButton mouseButton, bool mouseUp, int clickCount );
	void LoadUrl( const std::string& url );
	void SetHtml( const std::string& html );
	void Refresh();
	void Stop();
	void GoBack();
	void GoForward();
	void RunJavaScript( const std::string& code );
	void RegisterJavaScriptFunction( const std::string& objName, const std::string& funcName );
	void SetOpenLinksExternally( bool openLinksExternally );
	void ExecuteCallback( int callbackId, const JSValue& paramsArray );

public:
	//
	// CefClient interface 
	//
	CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override { return this; }
	CefRefPtr<CefLoadHandler> GetLoadHandler() override { return this; }
	CefRefPtr<CefDisplayHandler> GetDisplayHandler() override { return this; }
	CefRefPtr<CefRenderHandler> GetRenderHandler() override { return this; }
	CefRefPtr<CefRequestHandler> GetRequestHandler() override { return this; }
	CefRefPtr<CefContextMenuHandler> GetContextMenuHandler() override { return this; }
	CefRefPtr<CefDialogHandler> GetDialogHandler() override { return this; }
	CefRefPtr<CefJSDialogHandler> GetJSDialogHandler() override { return this; }

	bool OnProcessMessageReceived( CefRefPtr<CefBrowser>, CefRefPtr<CefFrame> frame, CefProcessId, CefRefPtr<CefProcessMessage> message ) override;

	//
	// CefLifeSpanHandler interface
	//
	void OnAfterCreated( CefRefPtr<CefBrowser> browser ) override;
	void OnBeforeClose( CefRefPtr<CefBrowser> browser ) override;
	bool OnBeforePopup( CefRefPtr<CefBrowser>,
		CefRefPtr<CefFrame> frame,
		const CefString& targetUrl,
		const CefString&,
		CefLifeSpanHandler::WindowOpenDisposition targetDisposition,
		bool,
		const CefPopupFeatures&,
		CefWindowInfo&,
		CefRefPtr<CefClient>&,
		CefBrowserSettings&,
		CefRefPtr<CefDictionaryValue>&,
		bool* ) override;

//
// CefLoadHandler interface
//
	void OnLoadStart( CefRefPtr<CefBrowser>, CefRefPtr<CefFrame> frame, CefLoadHandler::TransitionType ) override;
	void OnLoadEnd( CefRefPtr<CefBrowser>, CefRefPtr<CefFrame> frame, int httpStatusCode ) override;
	void OnLoadError( CefRefPtr<CefBrowser>, CefRefPtr<CefFrame> frame, CefLoadHandler::ErrorCode errorCode, const CefString& errorText, const CefString& failedURL ) override;

	//
	// CefDisplayHandler interface
	//
	void OnAddressChange( CefRefPtr<CefBrowser>, CefRefPtr<CefFrame> frame, const CefString& url ) override;
	bool OnConsoleMessage( CefRefPtr<CefBrowser>, cef_log_severity_t level, const CefString& message, const CefString& source, int line ) override;
	void OnTitleChange( CefRefPtr<CefBrowser>, const CefString& title ) override;

	//
	// CefRenderHandler interface
	//
	void OnCursorChange( CefRefPtr<CefBrowser>, CefCursorHandle, CefRenderHandler::CursorType chromeCursor, const CefCursorInfo& ) override;
	void GetViewRect( CefRefPtr<CefBrowser>, CefRect& rect ) override;
	void OnPopupShow( CefRefPtr<CefBrowser>, bool show ) override;
	void OnPopupSize( CefRefPtr<CefBrowser>, const CefRect& rect ) override;
	void OnPaint( CefRefPtr<CefBrowser>, CefRenderHandler::PaintElementType type, const CefRenderHandler::RectList& dirtyRects, const void* buffer, int width, int height ) override;

	//
	// CefRequestHandler interface
	//
	CefRefPtr<CefResourceRequestHandler> GetResourceRequestHandler( CefRefPtr<CefBrowser> browser,
		CefRefPtr<CefFrame> frame,
		CefRefPtr<CefRequest> request,
		bool is_navigation,
		bool is_download,
		const CefString& request_initiator,
		bool& disable_default_handling ) override {
		return this;
	}

	bool OnBeforeBrowse( CefRefPtr<CefBrowser>,
		CefRefPtr<CefFrame>,
		CefRefPtr<CefRequest> request,
		bool,
		bool ) override;

//
// CefResourceRequestHandler interface
//
	ReturnValue OnBeforeResourceLoad( CefRefPtr<CefBrowser>,
		CefRefPtr<CefFrame>,
		CefRefPtr<CefRequest> request,
		CefRefPtr<CefRequestCallback> ) override;

	void OnProtocolExecution( CefRefPtr<CefBrowser>,
		CefRefPtr<CefFrame>,
		CefRefPtr<CefRequest>,
		bool& allow_os_execution ) override;

//
// CefContextMenuHandler interface
//
	void OnBeforeContextMenu( CefRefPtr<CefBrowser>,
		CefRefPtr<CefFrame>,
		CefRefPtr<CefContextMenuParams>,
		CefRefPtr<CefMenuModel> model ) override;

//
// CefDialogHandler interface
//
	bool OnFileDialog( CefRefPtr<CefBrowser>,
		FileDialogMode,
		const CefString&,
		const CefString&,
		const std::vector<CefString>&,
		int,
		CefRefPtr<CefFileDialogCallback> callback ) override;

//
// CefJSDialogHandler
//
	bool OnJSDialog( CefRefPtr<CefBrowser>,
		const CefString&,
		JSDialogType,
		const CefString&,
		const CefString&,
		CefRefPtr<CefJSDialogCallback>,
		bool& suppress_message ) override;

	bool OnBeforeUnloadDialog( CefRefPtr<CefBrowser>,
		const CefString&,
		bool,
		CefRefPtr<CefJSDialogCallback> callback ) override;

private:
	int m_Wide;
	int m_Tall;

	// Store popup data separately so we can blit it on to the output image
	int m_PopupX;
	int m_PopupY;
	int m_PopupWide;
	int m_PopupTall;
	unsigned char* m_PopupData;

	// Messages for the main thread go here
	MessageQueue m_MessageQueue;

	ImageData m_ImageData;

	CefRefPtr<CefBrowser> m_Browser;
	CefRefPtr<CefBrowserHost> m_BrowserHost;

	bool m_OpenLinksExternally;

private:
	IMPLEMENT_REFCOUNTING( ChromiumBrowser );
};