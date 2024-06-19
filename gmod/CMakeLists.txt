set(TARGET gmod)
set(SOURCES
	Main.c)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--export-dynamic") # used for exposing the "has_namespace_support" variable for dlsym
add_executable(${TARGET} ${SOURCES})
target_link_libraries(${TARGET})
