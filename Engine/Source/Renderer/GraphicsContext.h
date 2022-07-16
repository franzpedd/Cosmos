#pragma once

#include "Core/Window.h"
#include "Util/Memory.h"

namespace Engine
{
	enum class RendererApi
	{
		NO_API = 0,
		VULKAN
	};

	class GraphicsContext
	{
	public:

		// returns a shared to a new context
		static SharedPointer<GraphicsContext> CreateGraphicsContext(Window& window, RendererApi api = RendererApi::VULKAN);

		// destructor
		virtual ~GraphicsContext() = default;

	};
}