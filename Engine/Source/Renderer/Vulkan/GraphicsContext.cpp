#include "enpch.h"
#include "GraphicsContext.h"

namespace Engine
{
	namespace Vulkan
	{
		GraphicsContext::GraphicsContext(Window& window)
			: m_Window(window)
		{
			ENGINE_TRACE("Creating Vulkan Graphics Context");

			ENGINE_ASSERT(volkInitialize() == VK_SUCCESS, "Failed to initialize volk");

			CreateInstance();
			m_Validations.Create(m_Instance);
		}

		GraphicsContext::~GraphicsContext()
		{
			m_Validations.Destroy(m_Instance);
			vkDestroyInstance(m_Instance, nullptr);
		}

		void GraphicsContext::CreateInstance()
		{
			ENGINE_ASSERT((m_Validations.AreEnabled() && !m_Validations.CheckValidationLayerSupport()) == false, "Validation layers requested but not available");

			VkApplicationInfo appInfo{};
			appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
			appInfo.pNext = nullptr;
			appInfo.pApplicationName = "Application";
			appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
			appInfo.pEngineName = "Cosmos";
			appInfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
			appInfo.apiVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);

			auto extensions = GetRequiredExtensions();
			auto validations = m_Validations.GetValidations();

			VkInstanceCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			createInfo.flags = 0;
			createInfo.pApplicationInfo = &appInfo;
			createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
			createInfo.ppEnabledExtensionNames = extensions.data();

			VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
			m_Validations.PopulateDebugMessengerCreateInfo(debugCreateInfo);

			if (m_Validations.AreEnabled())
			{
				createInfo.enabledLayerCount = static_cast<uint32_t>(validations.size());
				createInfo.ppEnabledLayerNames = validations.data();
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

		void GraphicsContext::PrintAvailableExtensions()
		{
			uint32_t extensionCount = 0;
			vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
			std::vector<VkExtensionProperties> extensions(extensionCount);
			vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

			for (const auto& ext : extensions) ENGINE_INFO("Available Extension: %s", ext.extensionName);
		}

		std::vector<const char*> GraphicsContext::GetRequiredExtensions()
		{
			uint32_t windowExtensionCount = 0;
			const char** windowExtensions = m_Window.GetInstanceExtensions(&windowExtensionCount);
			std::vector<const char*> extensions(windowExtensions, windowExtensions + windowExtensionCount);

			if (m_Validations.AreEnabled()) extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

			return extensions;
		}
	}
}