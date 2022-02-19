
#include "Window.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"

#ifdef _WIN32
	#include <Windows.h>
#endif


static void glfw_framebuffersize_callback( GLFWwindow* glfwWindow, int width, int height )
{
	auto* window = Window::FromInternal( glfwWindow );

	if ( window == nullptr )
		return;

	window->OnFramebufferResized( width, height );
}

//

std::unique_ptr<Window> Window::Create( const char* pTitle )
{
	GLFWwindow* glfwWindow;

	glfwWindowHint( GLFW_CLIENT_API, GLFW_OPENGL_API );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 0 );

	glfwWindow = glfwCreateWindow( 640, 480, pTitle, NULL, NULL );
	if ( !glfwWindow )
		return nullptr;

	glfwMakeContextCurrent( glfwWindow );
	glfwSwapInterval( 1 );

	if ( !gladLoadGLLoader( reinterpret_cast<GLADloadproc>( glfwGetProcAddress ) ) )
		return nullptr;

	return std::unique_ptr<Window>( new Window( glfwWindow ) );
}

Window* Window::FromInternal( GLFWwindow* glfwWindow )
{
	return static_cast<Window*>( glfwGetWindowUserPointer( glfwWindow ) );
}

//

Window::Window( GLFWwindow* glfwWindow )
	: _glfwWindow( glfwWindow )
{
	glfwSetWindowUserPointer( _glfwWindow, this );

	// GLFW Events
	glfwSetFramebufferSizeCallback( _glfwWindow, glfw_framebuffersize_callback );

	// Trigger some default events
	int width, height;
	glfwGetFramebufferSize( glfwWindow, &width, &height );
	OnFramebufferResized( width, height );
}

Window::~Window()
{
	glfwSetWindowUserPointer( _glfwWindow, nullptr );
	glfwDestroyWindow( _glfwWindow );
}

bool Window::ShouldClose()
{
	return glfwWindowShouldClose( _glfwWindow );
}

void Window::SwapBuffers()
{
	return glfwSwapBuffers( _glfwWindow );
}

void Window::PollEvents()
{
	glfwPollEvents();
}

GLFWwindow* Window::GetInternal()
{
	return _glfwWindow;
}

void Window::OnFramebufferResized( int width, int height )
{
	glViewport( 0, 0, width, height );
}