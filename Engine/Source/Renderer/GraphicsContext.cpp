#include "enpch.h"
#include "GraphicsContext.h"

#include "Vulkan/GraphicsContext.h"

namespace Engine
{
	UniquePointer<GraphicsContext> GraphicsContext::CreateGraphicsContext(Window& window, RendererApi api)
	{
		ENGINE_ASSERT(api == RendererApi::VULKAN, "Vulkan is the only API supported right now");
		return CreateUniquePointer<Vulkan::GraphicsContext>(window);
	}
}