#include "enpch.h"
#include "Context.h"

#include "Vulkan/VulkanContext.h"

namespace Engine::Renderer
{
	SharedPointer<Context> Context::Create(SharedPointer<Window>& window, API api)
	{
		ENGINE_ASSERT(api == API::Vulkan, "Vulkan is the only supported rendering api right now");
		return CreateSharedPointer<VulkanContext>(window);
	}
}