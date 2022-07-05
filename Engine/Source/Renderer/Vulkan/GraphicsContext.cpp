#include "enpch.h"
#include "GraphicsContext.h"

#define VOLK_IMPLEMENTATION // define this only once in the entire solution
#include <volk.h>

// enable validation layers and it's debug callback if support are supported
#ifdef ENGINE_DEBUG
const bool ValidationLayers = true;

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* callback, void* data)
{
	if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
	{
		ENGINE_TRACE("[VERBOSE]: %s", callback->pMessage);
		return VK_FALSE;
	}

	if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
	{
		ENGINE_TRACE("[INFO]: %s", callback->pMessage);
		return VK_FALSE;
	}

	if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
	{
		ENGINE_ASSERT("[WARN]: %s", callback->pMessage);
		return VK_FALSE;
	}

	if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
	{
		ENGINE_ASSERT(false, "[ERROR]: %s", callback->pMessage);
		return VK_FALSE;
	}
}
#else
const bool ValidationLayers = false;

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT severity, VkDebugUtilsMessageTypeFlagsEXT type, const VkDebugUtilsMessengerCallbackDataEXT* callback, void* data)
{
	return VK_FALSE;
}
#endif

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
			SetupDebugMessenger();
		}

		GraphicsContext::~GraphicsContext()
		{
			if (ValidationLayers) vkDestroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, nullptr);

			vkDestroyInstance(m_Instance, nullptr);
		}

		void GraphicsContext::CreateInstance()
		{
			ENGINE_ASSERT((ValidationLayers && !CheckValidationLayerSupport()) == false, "Validation layers requested but not available");

			VkApplicationInfo appInfo{};
			appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
			appInfo.pNext = nullptr;
			appInfo.pApplicationName = "Application";
			appInfo.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
			appInfo.pEngineName = "Cosmos";
			appInfo.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
			appInfo.apiVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);

			auto extensions = GetRequiredExtensions();

			VkInstanceCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			createInfo.flags = 0;
			createInfo.pApplicationInfo = &appInfo;
			createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
			createInfo.ppEnabledExtensionNames = extensions.data();

			VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
			PopulateDebugMessengerCreateInfo(debugCreateInfo);

			if (ValidationLayers)
			{
				createInfo.enabledLayerCount = static_cast<uint32_t>(m_ValidationLayers.size());
				createInfo.ppEnabledLayerNames = m_ValidationLayers.data();
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

		void GraphicsContext::SetupDebugMessenger()
		{
			if (!ValidationLayers) return;

			VkDebugUtilsMessengerCreateInfoEXT createInfo{};
			PopulateDebugMessengerCreateInfo(createInfo);

			ENGINE_ASSERT(vkCreateDebugUtilsMessengerEXT(m_Instance, &createInfo, nullptr, &m_DebugMessenger) == VK_SUCCESS, "Failed to setup debug the messenger");
		}

		void GraphicsContext::PrintAvailableExtensions()
		{
			uint32_t extensionCount = 0;
			vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
			std::vector<VkExtensionProperties> extensions(extensionCount);
			vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

			for (const auto& ext : extensions) ENGINE_INFO("Available Extension: %s", ext.extensionName);
		}

		bool GraphicsContext::CheckValidationLayerSupport()
		{
			uint32_t layerCount;
			vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

			std::vector<VkLayerProperties> availableLayers(layerCount);
			vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

			for (const char* name : m_ValidationLayers)
			{
				bool found = false;

				for (const auto& layer : availableLayers)
				{
					if (strcmp(name, layer.layerName) == 0)
					{
						found = true;
						break;
					}
				}

				if (!found) return false;
			}

			return true;
		}

		std::vector<const char*> GraphicsContext::GetRequiredExtensions()
		{
			uint32_t windowExtensionCount = 0;
			const char** windowExtensions = m_Window.GetInstanceExtensions(&windowExtensionCount);
			std::vector<const char*> extensions(windowExtensions, windowExtensions + windowExtensionCount);

			if (ValidationLayers) extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

			return extensions;
		}

		void GraphicsContext::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createinfo)
		{
			createinfo = {};
			createinfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
			createinfo.pNext = nullptr;
			createinfo.flags = 0;
			createinfo.messageSeverity =
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
				| VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
				| VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
				| VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
			createinfo.messageType =
				VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
				| VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
				| VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
			createinfo.pfnUserCallback = DebugCallback;
			createinfo.pUserData = nullptr;
		}
	}
}