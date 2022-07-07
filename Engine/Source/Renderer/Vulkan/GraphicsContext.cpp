#include "enpch.h"
#include "GraphicsContext.h"

namespace Engine
{
	namespace Vulkan
	{
		GraphicsContext::GraphicsContext(Window& window)
			: m_Window(window)
		{
			ENGINE_TRACE("Creating Vulkan Graphics Context");
			ENGINE_ASSERT(volkInitialize() == VK_SUCCESS, "Failed to initialize volk");


			m_Instance.Create(m_Window, m_Validations);
			m_Validations.Create(m_Instance.GetNativeInstance());
			m_Device.Create(m_Instance.GetNativeInstance(), m_Validations);
		}

		GraphicsContext::~GraphicsContext()
		{
			m_Device.Destroy();
			m_Validations.Destroy(m_Instance.GetNativeInstance());
			m_Instance.Destroy();
		}
	}
}