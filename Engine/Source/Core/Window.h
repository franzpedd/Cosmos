#pragma once

#include <cstdint>

#include "Utils/Memory.h"

struct GLFWwindow;

namespace Engine
{
	class Window
	{
	public:

		// returns a shared pointer to a new context
		static SharedPointer<Window> Create(const char* title, uint32_t width, uint32_t height);

		// constructor
		Window(const char* title, uint32_t width, uint32_t height);

		// destructor
		~Window();

		// returns the native window object
		inline GLFWwindow* GetNativeWindow() const { return m_Window; }

	public:

		// updates the window events
		void Update();

		// returns if the window should close
		bool ShouldClose();

	public:

		// returns a list of extensions needed to create an vulkan instance for glfw
		const char** GetInstanceExtensions(uint32_t* count);

		// returns the framebuffer size in pixels
		std::pair<int, int> GetFramebufferSize();

		// returns the current window's width
		inline uint32_t GetWidth() const { return m_Width; }

		// returns the current window's height
		inline uint32_t GetHeight() const { return m_Height; }

	private:

		static uint32_t m_WindowCount;
		GLFWwindow* m_Window;
		const char* m_Title;
		uint32_t m_Width;
		uint32_t m_Height;

	};
}