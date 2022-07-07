#pragma once

#include <volk.h>

#include "Core/Window.h"
#include "Validations.h"

namespace Engine
{
	namespace Vulkan
	{
		class Instance
		{
		public:

			// constructor
			Instance() = default;

			// destructor
			~Instance() = default;

			// returns a reference to the vulkan instance
			inline VkInstance& GetNativeInstance() { return m_Instance; }

		public:

			// creates the vulkan instance
			void Create(Window& window, Validations& validations);

			// destroys the vulkan instance
			void Destroy();

		private:

			// prints out the available extensions on instance creation
			void PrintAvailableExtensions();

			// returns a list of extensions
			std::vector<const char*> GetRequiredExtensions(Window& window, Validations& validations);

		private:

			VkInstance m_Instance = VK_NULL_HANDLE;

		};
	}
}