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
			m_GraphicsPipeline = GraphicsPipeline::Create(*m_Device);
		}

		GraphicsContext::~GraphicsContext()
		{
			// deleting members are not neccessary as
			// all of them are currently smart pointers
		}
	}
}