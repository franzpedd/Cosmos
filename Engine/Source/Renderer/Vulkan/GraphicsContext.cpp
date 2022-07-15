#include "enpch.h"
#include "GraphicsContext.h"

namespace Engine
{
	namespace Vulkan
	{
		GraphicsContext::GraphicsContext(Window& window) : m_Window(window)
		{
			ENGINE_TRACE("Creating Vulkan Graphics Context");

			m_Instance = Instance::Create(m_Window);
			m_Device = Device::Create(m_Window, *m_Instance);
			// next to do is swapchain, right now inside device
		}

		GraphicsContext::~GraphicsContext()
		{
			// this technically isn't needed as it's a smart pointer
			//m_Device.reset();
			//m_Instance.reset();
		}
	}
}