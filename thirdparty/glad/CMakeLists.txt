set(SOURCES
	src/blank.cpp
	src/glad.c
	include/glad/glad.h
	include/KHR/khrplatform.h)

add_library(glad STATIC ${SOURCES})
target_include_directories(glad PUBLIC include)
