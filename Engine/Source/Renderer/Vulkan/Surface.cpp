#include "enpch.h"
#include "Surface.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW\glfw3.h>

namespace Engine::Renderer
{
	SharedPointer<Surface> Surface::Create(SharedPointer<Window>& window, SharedPointer<Instance>& instance)
	{
		return CreateSharedPointer<Surface>(window, instance);
	}

	Surface::Surface(SharedPointer<Window>& window, SharedPointer<Instance>& instance)
		: m_Window(window), m_Instance(instance)
	{
		ENGINE_TRACE("Creating Surface");

		ENGINE_ASSERT(glfwCreateWindowSurface(m_Instance->GetNativeVulkanInstance(), m_Window->GetNativeWindow(), nullptr, &m_Surface) == VK_SUCCESS, "Failed to create vulkan surface")
	}

	Surface::~Surface()
	{
		vkDestroySurfaceKHR(m_Instance->GetNativeVulkanInstance(), m_Surface, nullptr);
	}
}