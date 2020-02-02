#include <Windows.h>
#include <stdio.h>

#include "Window.h"
#include "HtmlSystemLoader.h"
#include "HtmlPanel.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#if defined( _WIN32 ) && defined( NDEBUG )
	#include "include/cef_sandbox_win.h"

	extern "C"
	{
		__declspec( dllexport ) void* CreateCefSandboxInfo()
		{
			return cef_sandbox_info_create();
		}

		__declspec( dllexport ) void DestroyCefSandboxInfo( void* info )
		{
			cef_sandbox_info_destroy( info );
		}
	}
#endif

static void glfw_error_callback( int error, const char* description )
{
	fprintf( stderr, "Glfw Error %d: %s\n", error, description );
}

int WINAPI WinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow )
{
	// Sub-process
#ifdef _WIN32
	if ( strstr( lpCmdLine, "--type=" ) )
	{
		int ChromiumMain( HINSTANCE hInstance );

		int exit_code = ChromiumMain( hInstance );

		if ( exit_code != -1 )
		{
			return exit_code;
		}
	}
#endif

	if ( !HtmlSystem_Init() )
		return -1;

	glfwSetErrorCallback( glfw_error_callback );

	if ( !glfwInit() )
		return -1;

	{
		auto window = Window::Create( "Example Host" );
		if ( !window )
		{
			glfwTerminate();
			return -1;
		}

		auto panel = std::unique_ptr<HtmlPanel>( new HtmlPanel() );
		panel->LoadUrl( "https://google.com/" );

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL( window->GetInternal(), true );
		ImGui_ImplOpenGL3_Init( "#version 130" );
		ImGui::GetIO().IniFilename = nullptr;

		while ( !window->ShouldClose() )
		{
			HtmlSystem_Tick();
			window->PollEvents();

			glClearColor( 0.f, 0.f, 0.f, 1.f );
			glClear( GL_COLOR_BUFFER_BIT );

			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
		
			panel->Render();
			
			ImGui::Render();
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