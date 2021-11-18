set(SOURCES
	StubClient.cpp
	StubClient.h
	StubSystem.cpp
	StubSystem.h)

add_library(html_stub SHARED ${SOURCES})
target_link_libraries(html_stub html)
SET_TARGET_PROPERTIES(html_stub PROPERTIES PREFIX "")

if(UNIX)
	set_target_properties(html_stub PROPERTIES OUTPUT_NAME "html_stub_client")
endif()
install(TARGETS html_stub LIBRARY DESTINATION ${GAME_BIN_DIR})
