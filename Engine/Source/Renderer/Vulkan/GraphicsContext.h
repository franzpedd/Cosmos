#pragma once

#include <volk.h>

#include "Renderer/GraphicsContext.h"
#include "Validations.h"

namespace Engine
{
	namespace Vulkan
	{
		class GraphicsContext : public Engine::GraphicsContext
		{
		public:

			// constructor
			GraphicsContext(Window& window);

			// destructor
			virtual ~GraphicsContext();

		private:

			// creates the vulkan instance
			void CreateInstance();

		private:

			// prints out the available extensions on instance creation
			void PrintAvailableExtensions();

			// returns a list of extensions
			std::vector<const char*> GetRequiredExtensions();

		private:

			Window& m_Window;
			VkInstance m_Instance = nullptr;
			Validations m_Validations;

		};
	}
}