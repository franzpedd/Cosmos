#pragma once

#include "Core/Window.h"
#include "Util/Memory.h"

namespace Engine
{
	class GraphicsContext
	{
	public:

		enum class API
		{
			NO_API = 0,
			VULKAN
		};

	public:

		// returns a pointer to a new context
		static UniquePointer<GraphicsContext> CreateGraphicsContext(Window& window, API api = API::VULKAN);

		// destructor
		virtual ~GraphicsContext() = default;

	};
}