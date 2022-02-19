
#include <stdio.h>
#include <iostream>

#ifndef IMGUI_IMPL_OPENGL_LOADER_GLAD
#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#endif

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "Window.h"
#include "HtmlSystemLoader.h"
#include "HtmlPanel.h"


static void glfw_error_callback( int error, const char* description )
{
	fprintf( stderr, "Glfw Error %d: %s\n", error, description );
}

#ifdef _WIN32
int WINAPI WinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow ) {

	if ( strstr( lpCmdLine, "--type=" ) )
	{
		int ChromiumMain( HINSTANCE hInstance );

		int exit_code = ChromiumMain( hInstance );

		if ( exit_code != -1 )
		{
			return exit_code;
		}
	}
#else
int main( int argc, char** argv ) {
#endif

	if ( !HtmlSystem_Init() ) {
		std::cout << "Failed to initialize HtmlSystem" << std::endl;
		return -1;
	}

	glfwSetErrorCallback( glfw_error_callback );
	if ( !glfwInit() ){
		std::cout << "Failed to initialize GLFW" << std::endl;
		return -1;
	}

	{
		auto window = Window::Create( "Example Host" );
		if ( !window )
		{
			std::cout << "Failed to create window" << std::endl;
			glfwTerminate();
			return -1;
		}

		auto panel = std::unique_ptr<HtmlPanel>( new HtmlPanel() );
		panel->LoadUrl( "https://google.com/" );

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();
		ImGui::GetIO().IniFilename = nullptr;

		ImGui_ImplGlfw_InitForOpenGL( window->GetInternal(), true );
		ImGui_ImplOpenGL3_Init( "#version 130" );

		while ( !window->ShouldClose() )
		{
			HtmlSystem_Tick();
			window->PollEvents();

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
		
			panel->Render();

			ImGui::Render();

			int display_w, display_h;
			glfwGetFramebufferSize( window->GetInternal(), &display_w, &display_h );
			glViewport( 0, 0, display_w, display_h );
			glClearColor( 0.f, 0.f, 0.f, 1.f );
			glClear( GL_COLOR_BUFFER_BIT );

			ImGui_ImplOpenGL3_RenderDrawData( ImGui::GetDrawData() );

			window->SwapBuffers();
		}
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
	HtmlSystem_Shutdown();

	return 0;
}