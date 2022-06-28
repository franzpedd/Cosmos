#include "enpch.h"
#include "Window.h"

#include <GLFW/glfw3.h>

namespace Engine
{
	Window::Window(const char* title, uint32_t width, uint32_t height)
		: m_Window(nullptr), m_Title(title), m_Width(width), m_Height(height)
	{

	}

	Window::~Window()
	{

	}
}