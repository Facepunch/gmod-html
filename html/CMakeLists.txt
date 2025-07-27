set(SOURCES
	html/IHtmlClient.h
	html/IHtmlClientListener.h
	html/IHtmlResourceHandler.h
	html/IHtmlSystem.h
	html/JSvalue.h)

add_library(html INTERFACE)
target_include_directories(html INTERFACE ./)

# Let us see the interface in Visual Studio
if(MSVC)
	add_custom_target(html.interface SOURCES ${SOURCES})
endif()
