#pragma once

#include "Core/Window.h"

#include <volk.h>
#include <vector>

namespace Engine::Renderer
{
	class Instance
	{
	public:

		// returns a smart pointer to a new instance
		static SharedPointer<Instance> Create(SharedPointer<Window>& window);

		// constructor
		Instance(SharedPointer<Window>& window);

		// destructor
		~Instance();

		// returns the native vulkan instance
		inline VkInstance& GetNativeVulkanInstance() { return m_Instance; }

	public:

		// returns the validations list
		inline const std::vector<const char*>& GetValidations() const { return m_Validations; }

		// returns if validations are enabled or not
		inline const bool AreValidationsEnabled() { return m_ValidationsEnabled; }

		// returns if required validations are supported
		bool CheckValidationSupport();

	private:

		// creates the instance
		void CreateInstance();

		// creates the validation layer
		void CreateValidations();

		// returns a list of required extensions
		std::vector<const char*> GetRequiredExtensions(SharedPointer<Window>& window);

	private:

		SharedPointer<Window>& m_Window;
		VkInstance m_Instance = VK_NULL_HANDLE;
		VkDebugUtilsMessengerEXT m_DebugMessenger = VK_NULL_HANDLE;
		const std::vector<const char*> m_Validations = { "VK_LAYER_KHRONOS_validation" };
#if defined ENGINE_DEBUG
		const bool m_ValidationsEnabled = true;
#else
		const bool m_ValidationsEnabled = false;
#endif
	};
}