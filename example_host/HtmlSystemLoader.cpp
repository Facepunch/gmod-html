#include <string>

#ifdef _WIN32
	#include <Windows.h>
	#include <direct.h>
#endif

#ifdef __linux__
	#include <stdlib.h>
	#include <string.h>
	#include <dlfcn.h>
	#include <unistd.h>
	#include <limits.h>
#endif

#include "HtmlSystemLoader.h"
#include "HtmlResourceHandler.h"

HtmlResourceHandler g_ResourceHandler;
IHtmlSystem* g_pHtmlSystem;

//
// Platform specific shit to dynamically find our html lib
//
bool HtmlSystem_Init()
{
#ifdef _WIN32
	HMODULE	hDLL = LoadLibrary( "html_chromium.dll" );

	if ( hDLL == nullptr )
		return false;

	IHtmlSystem** ppHtmlSystem = reinterpret_cast<IHtmlSystem**>( GetProcAddress( hDLL, "g_pHtmlSystem" ) );

	if ( ppHtmlSystem == nullptr || *ppHtmlSystem == nullptr )
		return false;

	g_pHtmlSystem = *ppHtmlSystem;

	char pPath[MAX_PATH] = { 0 };
	if ( _getcwd( pPath, sizeof( pPath ) ) != pPath )
		return false;

	std::string finalPath( pPath );
	finalPath.append( "/../../" ); // This has to point to where our 'hl2.exe' would live

	return g_pHtmlSystem->Init( finalPath.c_str(), &g_ResourceHandler );
#elif __linux__
	void* library = dlopen( "html_chromium_client.so", RTLD_LAZY );

	if ( library == nullptr )
		return false;

	IHtmlSystem** ppHtmlSystem = reinterpret_cast<IHtmlSystem**>( dlsym( library, "g_pHtmlSystem" ) );

	if ( ppHtmlSystem == nullptr || *ppHtmlSystem == nullptr )
		return false;

	g_pHtmlSystem = *ppHtmlSystem;

	char pPath[PATH_MAX] = { 0 };
	if ( getcwd( pPath, sizeof( pPath ) ) == NULL )
		return false;
	strcat(pPath, "/../../");

	return g_pHtmlSystem->Init( canonicalize_file_name(pPath), &g_ResourceHandler );
#else
	#error
#endif
}

void HtmlSystem_Tick()
{
	g_pHtmlSystem->Update();
}

void HtmlSystem_Shutdown()
{
	g_pHtmlSystem->Shutdown();
	g_pHtmlSystem = nullptr;
}
