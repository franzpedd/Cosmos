#include "enpch.h"
#include "Surface.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace Engine
{
	namespace Vulkan
	{
		void Surface::Create(Window& window, VkInstance& instance)
		{
			ENGINE_ASSERT(glfwCreateWindowSurface(instance, window.GetNativeWindow(), nullptr, &m_Surface) == VK_SUCCESS, "Failed to create window surface");
		}

		void Surface::Destroy(VkInstance& instance)
		{
			vkDestroySurfaceKHR(instance, m_Surface, nullptr);
		}
	}
}