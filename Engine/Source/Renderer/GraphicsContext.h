#pragma once

#include "Core/Window.h"
#include "Util/Memory.h"

namespace Engine
{
	class GraphicsContext
	{
	public:

		enum class RendererApi
		{
			NO_API = 0,
			VULKAN
		};

	public:

		// returns a pointer to a new context
		static UniquePointer<GraphicsContext> CreateGraphicsContext(Window& window, RendererApi api = RendererApi::VULKAN);

		// destructor
		virtual ~GraphicsContext() = default;

	};
}