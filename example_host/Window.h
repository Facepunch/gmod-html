
#pragma once

#include <memory>

struct GLFWwindow;

class Window
{
public:
	Window( GLFWwindow* glfwWindow );
	~Window();

	Window( const Window& ) = delete;
	Window( Window&& ) = delete;

	Window& operator=( const Window& ) = delete;
	Window& operator=( Window&& ) = delete;

private:
	GLFWwindow* _glfwWindow;
	int _viewportWidth;
	int _viewportHeight;

public:
	static std::unique_ptr<Window> Create( const char* pTitle );
	static Window* FromInternal( GLFWwindow* glfwWindow );

	bool ShouldClose();
	void SwapBuffers();
	void PollEvents();

	GLFWwindow* GetInternal();

	int GetViewportWidth() { return _viewportWidth; }
	int GetViewportHeight() { return _viewportHeight; }

public:
	void OnFramebufferResized( int width, int height );
};
