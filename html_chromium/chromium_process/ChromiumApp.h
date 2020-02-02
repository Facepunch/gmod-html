#include "include/cef_app.h"

#include <vector>
#include <unordered_map>

class ChromiumApp
    : public CefApp
    , public CefRenderProcessHandler
    , public CefV8Handler
{
public:
    //
    // CefApp interface
    //
    void OnBeforeCommandLineProcessing( const CefString& process_type, CefRefPtr<CefCommandLine> command_line ) override;
    void OnRegisterCustomSchemes( CefRawPtr<CefSchemeRegistrar> registrar ) override;
    CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() override;
    
    //
    // CefRenderProcessHandler interface
    //
    void OnContextCreated( CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context ) override;
    void OnContextReleased( CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context ) override;
	bool OnProcessMessageReceived( CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefProcessId source_process, CefRefPtr<CefProcessMessage> message ) override;

    //
    // CefV8Handler interface
    //
    bool Execute( const CefString& name, CefRefPtr<CefV8Value> object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception ) override;

private:
    int CreateCallback( CefRefPtr<CefV8Context> context, CefRefPtr<CefV8Value> func );
    void RegisterFunctionInFrame( CefRefPtr<CefFrame> frame, const CefString& objName, const CefString& funcName );

    // Messages from the game process
	void ExecuteJavaScript( CefRefPtr<CefBrowser> browser, CefRefPtr<CefListValue> args );
    void RegisterFunction( CefRefPtr<CefBrowser> browser, CefRefPtr<CefListValue> args );
    void ExecuteCallback( CefRefPtr<CefBrowser> browser, CefRefPtr<CefListValue> args );
    void ForgetCallback( CefRefPtr<CefBrowser> browser, CefRefPtr<CefListValue> args );

private:
    std::vector<std::pair<CefString, CefString>> m_RegisteredFunctions;

    struct Callback
    {
        CefRefPtr<CefV8Context> Context;
        CefRefPtr<CefV8Value> Function;
    };

    std::unordered_map<int, Callback> m_Callbacks;
    int m_NextCallbackId = 0;

private:
    IMPLEMENT_REFCOUNTING( ChromiumApp );

};
