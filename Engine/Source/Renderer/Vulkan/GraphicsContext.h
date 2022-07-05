#pragma once

#include "Renderer/GraphicsContext.h"

#include <volk.h>

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

			// sets-up the vulkan debug messenger
			void SetupDebugMessenger();

		private:

			// prints out the available extensions on instance creation
			void PrintAvailableExtensions();

			// checks if all validation layers required are supported
			bool CheckValidationLayerSupport();

			// returns a list of extensions based on layers on or off
			std::vector<const char*> GetRequiredExtensions();

			// populates a VkDebugUtilsMessengerCreateInfoEXT
			void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createinfo);

		private:

			Window& m_Window;
			VkInstance m_Instance = nullptr;

			// validation an debugging
			const std::vector<const char*> m_ValidationLayers = { "VK_LAYER_KHRONOS_validation" };
			VkDebugUtilsMessengerEXT m_DebugMessenger;
		};
	}
}