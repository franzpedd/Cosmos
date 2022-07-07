#include "enpch.h"
#include "Window.h"

#include <GLFW/glfw3.h>

namespace Engine
{
	uint32_t Window::m_WindowCount = 0;

	Window::Window(const char* title, uint32_t width, uint32_t height)
		: m_Window(nullptr), m_Title(title), m_Width(width), m_Height(height)
	{
		ENGINE_TRACE("Creating Window");

		if (m_WindowCount >= 0)
		{
			ENGINE_ASSERT(glfwInit() == 1, "Could not initialize GLFW");
			ENGINE_ASSERT(glfwVulkanSupported() == 1, "Vulkan is not supported on this device");
		}

		m_WindowCount++;

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		m_Window = glfwCreateWindow(m_Width, m_Height, m_Title, nullptr, nullptr);
		ENGINE_ASSERT(m_Window != nullptr, "GLFW window creation has failed");

	}

	Window::~Window()
	{
		m_WindowCount--;

		if (m_WindowCount == 0)
		{
			glfwTerminate();
		}
	}

	void Window::Update()
	{
		glfwPollEvents();
	}

	bool Window::ShouldClose()
	{
		return glfwWindowShouldClose(m_Window);
	}

	const char** Window::GetInstanceExtensions(uint32_t* count)
	{
		return glfwGetRequiredInstanceExtensions(count);
	}
}