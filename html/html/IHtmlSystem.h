#pragma once

#include "IHtmlClient.h"
#include "IHtmlClientListener.h"
#include "IHtmlResourceHandler.h"

//
// The IHtmlSystem is a singleton that creates IHtmlClient instances and ticks them
//
class IHtmlSystem
{
public:
	virtual bool Init( const char* pBaseDir, IHtmlResourceHandler* pResourceHandler ) = 0;
	virtual void Shutdown() = 0;

	// Creates a new web client. This can do some asynchronous stuff so it shouldn't be slow.
	virtual IHtmlClient* CreateClient( IHtmlClientListener* pListener ) = 0;

	// Call this each frame, probably
	virtual void Update() = 0;

	// Create a representation of a JavaScript array
	// Invalidates all supplied JSValue instances
	virtual JSValue CreateArray( JSValue* pValues, size_t size ) = 0;

	// Create a representation of a JavaScript string
	virtual JSValue CreateString( const char* pValue, size_t size ) = 0;

	// Create a representation of a JavaScript HashMap (object)
	// Invalidates all supplied JSValue instances
	virtual JSValue CreateHashMap( const char** pKeys, const size_t* pKeySizes, JSValue* pValues, size_t size ) = 0;
};

//
// The HtmlSystem should be exported with this
//
#if defined( _WIN32 )
	#define HTMLSYSTEM_EXPORT(sys) \
		extern "C" __declspec( dllexport ) IHtmlSystem* g_pHtmlSystem = &sys;
#elif defined( __APPLE__ ) || defined( __linux__ )
	#define HTMLSYSTEM_EXPORT(sys) \
		extern "C" { __attribute__ ((visibility("default"))) IHtmlSystem* g_pHtmlSystem = &sys; }
#else
	#error HTMLSYSTEM_EXPORT not defined for platform
#endif


// Aaaaaaaaaaa
