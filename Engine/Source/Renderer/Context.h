#pragma once

#include "Core/Window.h"
#include "Utils/Memory.h"

namespace Engine::Renderer
{
	enum class API
	{
		Unknown = 0,
		Vulkan
	};

	class Context
	{
	public:

		// returns a shared pointer to a new context
		static SharedPointer<Context> Create(SharedPointer<Window>& window, API api = API::Vulkan);

		// destructor
		virtual ~Context() = default;

		// returns a pointer to the native context
		virtual Context& GetContext() = 0;

	public:

		// updates the renderer
		virtual void Update() = 0;

		// stop all pending graphics related functionality
		virtual void Finish() = 0;

	};
}