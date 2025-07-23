#include "StubSystem.h"
#include "StubClient.h"

bool StubSystem::Init( const char* pBaseDir, IHtmlResourceHandler* pResourceHandler )
{
	return true;
}

void StubSystem::Shutdown()
{

}

void StubSystem::Update()
{

}

IHtmlClient* StubSystem::CreateClient( IHtmlClientListener* pListener )
{
	return new StubClient();
}

JSValue StubSystem::CreateArray( JSValue* pValues, size_t size )
{
	return JSValue();
}

JSValue StubSystem::CreateString( const char* pValue, size_t size ) {
	return JSValue();
}

JSValue StubSystem::CreateHashMap( const char** pKeys, const size_t* pKeySizes, JSValue* pValues, size_t size )
{
	return JSValue();
}

static StubSystem g_StubSystem;
HTMLSYSTEM_EXPORT( g_StubSystem );