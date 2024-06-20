
set(TARGET gmod_launcher)
set(SOURCES
	Linux.cpp)

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--export-dynamic") # used for exposing the "has_namespace_support" variable for dlsym
add_executable(${TARGET} ${SOURCES})
target_link_libraries(${TARGET} ${CMAKE_DL_LIBS})

install(TARGETS ${TARGET} DESTINATION "${INSTALL_OUT_DIR}/GarrysMod/bin/${CEF_PLATFORM}")
set_target_properties(${TARGET} PROPERTIES INSTALL_RPATH "$ORIGIN")
set_target_properties(${TARGET} PROPERTIES BUILD_WITH_INSTALL_RPATH TRUE)
set_target_properties(${TARGET} PROPERTIES OUTPUT_NAME "gmod")
