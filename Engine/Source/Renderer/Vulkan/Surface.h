#pragma once

#include "Instance.h"

#include <volk.h>

namespace Engine::Renderer
{
	class Surface
	{
	public:

		// returns a smart pointer to a new surface
		static SharedPointer<Surface> Create(SharedPointer<Window>& window, SharedPointer<Instance>& instance);

		// constructor
		Surface(SharedPointer<Window>& window, SharedPointer<Instance>& instance);

		// destructor
		~Surface();

		// returns the native vulkan surface
		inline VkSurfaceKHR& GetNativeVulkanSurface() { return m_Surface; }

	public:



	private:

		SharedPointer<Window>& m_Window;
		SharedPointer<Instance>& m_Instance;
		VkSurfaceKHR m_Surface = VK_NULL_HANDLE;

	};
}