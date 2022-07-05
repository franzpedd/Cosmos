#include "enpch.h"
#include "GraphicsContext.h"
#include "Core/Logger.h"

#include "Vulkan/GraphicsContext.h"

namespace Engine
{
	UniquePointer<GraphicsContext> GraphicsContext::CreateGraphicsContext(Window& window, API api)
	{
		ENGINE_ASSERT(api == API::VULKAN, "Vulkan is the only API supported right now");
		return CreateUniquePointer<Vulkan::GraphicsContext>(window);
	}
}