set(SOURCES
	imconfig.h
	imgui.cpp
	imgui.h
	imgui_draw.cpp
	imgui_impl_glfw.cpp
	imgui_impl_glfw.h
	imgui_impl_opengl3.cpp
	imgui_impl_opengl3.h
	imgui_widgets.cpp
	imgui_internal.h
	imstb_rectpack.h
	imstb_textedit.h
	imstb_truetype.h)

add_library(imgui STATIC ${SOURCES})
target_link_libraries(imgui glfw glad)
target_include_directories(imgui PUBLIC ./)
