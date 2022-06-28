#pragma once

#include <cstdint>

struct GLFWwindow;

namespace Engine
{
	class Window
	{
	public:

		// constructor
		Window(const char* title, uint32_t width, uint32_t height);

		// destructor
		~Window();

	private:

		GLFWwindow* m_Window;
		const char* m_Title;
		uint32_t m_Width;
		uint32_t m_Height;

	};
}