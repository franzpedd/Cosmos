#include "enpch.h"
#include "Shader.h"

#include "Vulkan/Shader.h"

namespace Engine
{
	SharedPointer<Shader> Shader::Create(Specification& specs, RendererApi api)
	{
		ENGINE_ASSERT(api == RendererApi::VULKAN, "Vulkan is the only API supported right now");
		return CreateSharedPointer<Vulkan::Shader>(specs);
	}
}