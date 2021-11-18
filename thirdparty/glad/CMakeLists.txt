
set(SOURCES
	src/blank.cpp
	src/glad.c
	include/glad/glad.h
	include/KHR/khrplatform.h)

set_source_files_properties(src/glad.c PROPERTIES LANGUAGE CXX)

add_library(glad STATIC ${SOURCES})
target_include_directories(glad PUBLIC include)
