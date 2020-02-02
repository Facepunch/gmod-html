#pragma once

#include "html/IHtmlSystem.h"

#include "cef_start.h"
#include "include/cef_base.h"
#include "include/base/cef_lock.h"
#include "cef_end.h"

#include <vector>

class ChromiumClient;
class ResourceHandler;

class ChromiumSystem : public IHtmlSystem
{
public:
	bool Init( const char* pBaseDir, IHtmlResourceHandler* pResourceHandler ) override;
	void Shutdown() override;

	IHtmlClient* CreateClient( IHtmlClientListener* pListener ) override;

	void Update() override;

	void OnClientOpen( ChromiumClient* client );
	void OnClientClose( ChromiumClient* client );

	void QueueRequest( CefRefPtr<ResourceHandler> resourceHandler );

	JSValue CreateArray( JSValue* pValues, size_t size ) override;
	JSValue CreateString( const char* pValue, size_t size ) override;
	JSValue CreateHashMap( const char** pKeys, const size_t* pKeySizes, JSValue* pValues, size_t size ) override;

private:
	std::vector<ChromiumClient*> m_ActiveClients;

	base::Lock m_RequestsLock;
	std::vector<CefRefPtr<ResourceHandler>> m_Requests;
};

// We've got a few bits of code that need to access this directly
extern ChromiumSystem g_ChromiumSystem;
