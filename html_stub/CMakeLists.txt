set(SOURCES
	StubClient.cpp
	StubClient.h
	StubSystem.cpp
	StubSystem.h)

add_library(html_stub SHARED ${SOURCES})
target_link_libraries(html_stub html)
SET_TARGET_PROPERTIES(html_stub PROPERTIES PREFIX "")
