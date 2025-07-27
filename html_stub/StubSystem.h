#pragma once

#include "html/IHtmlSystem.h"

class StubClient;

class StubSystem : public IHtmlSystem
{
public:
	bool Init( const char* pBaseDir, IHtmlResourceHandler* pResourceHandler ) override;
	void Shutdown() override;
	void Update() override;

	IHtmlClient* CreateClient( IHtmlClientListener* pListener ) override;

	JSValue CreateArray( JSValue* pValues, size_t size ) override;
	JSValue CreateString( const char* pValue, size_t size ) override;
	JSValue CreateHashMap( const char** pKeys, const size_t* pKeySizes, JSValue* pValues, size_t size ) override;
};
