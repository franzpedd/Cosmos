#include "enpch.h"
#include "GraphicsContext.h"

#include "Vulkan/GraphicsContext.h"

namespace Engine
{
	SharedPointer<GraphicsContext> GraphicsContext::CreateGraphicsContext(Window& window, RendererApi api)
	{
		ENGINE_ASSERT(api == RendererApi::VULKAN, "Vulkan is the only API supported right now");
		return CreateSharedPointer<Vulkan::GraphicsContext>(window);
	}
}