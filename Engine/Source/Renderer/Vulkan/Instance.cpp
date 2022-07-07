#include "enpch.h"
#include "Instance.h"

namespace Engine
{
	namespace Vulkan
	{
		void Instance::Create(Window& window, Validations& validations)
		{
			VkApplicationInfo appInfo{};
			appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
			appInfo.pNext = nullptr;
			appInfo.pApplicationName = "Application";
			appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
			appInfo.pEngineName = "Cosmos";
			appInfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
			appInfo.apiVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);

			auto extensions = GetRequiredExtensions(window, validations);
			auto validationlayers = validations.GetValidations();

			VkInstanceCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			createInfo.flags = 0;
			createInfo.pApplicationInfo = &appInfo;
			createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
			createInfo.ppEnabledExtensionNames = extensions.data();

			VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
			validations.PopulateDebugMessengerCreateInfo(debugCreateInfo);

			if (validations.AreEnabled())
			{
				createInfo.enabledLayerCount = static_cast<uint32_t>(validationlayers.size());
				createInfo.ppEnabledLayerNames = validationlayers.data();
				createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
			}

			else
			{
				createInfo.enabledLayerCount = 0;
				createInfo.ppEnabledLayerNames = nullptr;
				createInfo.pNext = nullptr;
			}

			PrintAvailableExtensions();

			ENGINE_ASSERT(vkCreateInstance(&createInfo, nullptr, &m_Instance) == VK_SUCCESS, "Failed to Create Vulkan Instance");
			volkLoadInstance(m_Instance);
		}

		void Instance::Destroy()
		{
			vkDestroyInstance(m_Instance, nullptr);
		}

		void Instance::PrintAvailableExtensions()
		{
			uint32_t extensionCount = 0;
			vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
			std::vector<VkExtensionProperties> extensions(extensionCount);
			vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

			for (const auto& ext : extensions) ENGINE_INFO("Available Extension: %s", ext.extensionName);
		}

		std::vector<const char*> Instance::GetRequiredExtensions(Window& window, Validations& validations)
		{
			uint32_t windowExtensionCount = 0;
			const char** windowExtensions = window.GetInstanceExtensions(&windowExtensionCount);
			std::vector<const char*> extensions(windowExtensions, windowExtensions + windowExtensionCount);

			if (validations.AreEnabled()) extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

			return extensions;
		}
	}
}