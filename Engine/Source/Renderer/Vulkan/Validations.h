#pragma once

#include <volk.h>

namespace Engine
{
	namespace Vulkan
	{
		class Validations
		{
		public:

			// constructor
			Validations() = default;

			// destructor
			~Validations() = default;

			// returns either if validation layers are enabled or not
			inline bool AreEnabled() { return AreValidationEnabled; }

			// returns the list of layers
			inline std::vector<const char*> GetValidations() { return m_ValidationLayers; }

		public:

			// creates the validation layers using vulkan c-functions
			void Create(VkInstance& instance);

			// destroys the validation layers object using vulkan c-funcions
			void Destroy(VkInstance& instance);

			// populates a debug messenger create info struct
			void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& object);

			// checks if all validation layers required are supported
			bool CheckValidationLayerSupport();

		private:

			const std::vector<const char*> m_ValidationLayers = { "VK_LAYER_KHRONOS_validation" };
			VkDebugUtilsMessengerEXT m_DebugMessenger = nullptr;

#if defined ENGINE_DEBUG
			const bool AreValidationEnabled = true;
#else
			const bool AreValidationEnabled = false;
#endif

		};
	}
}