#pragma once

#include <volk.h>

#include "Core/Window.h"
#include "Util/Memory.h"

namespace Engine
{
	namespace Vulkan
	{
		class Instance
		{
		public:

			// returns a pointer to a new instance class
			static UniquePointer<Instance> Create(Window& window);

			// constructor
			Instance(Window& window);

			// destructor
			~Instance();

		public:

			// returns a reference for the validations list
			inline const std::vector<const char*>& GetValidations() const { return m_Validations; }

			// returns a reference to the native vulkan instance
			inline VkInstance& GetNativeInstance() { return m_Instance; }

		private:

			// creates the instance
			void CreateInstance(Window& window);

			// creates the validations
			void CreateValidations();

		public:

			// returns if validations are currently enabled
			inline const bool AreValidationsEnabled() const { return m_EnableValidations; }

			// returns if required validations are supported
			bool CheckValidationsSupport();

		private:

			// returns a list of required extensions
			std::vector<const char*> GetRequiredExtensions(Window& window);

		private:

			Window& m_Window;
			VkInstance m_Instance;
			VkDebugUtilsMessengerEXT m_DebugMessenger{ VK_NULL_HANDLE };
			const std::vector<const char*> m_Validations{ "VK_LAYER_KHRONOS_validation" };
#if defined ENGINE_DEBUG
			const bool m_EnableValidations = true;
#else
			const bool m_EnableValidations = false;
#endif
		};

	}
}