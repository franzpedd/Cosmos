#include "enpch.h"
#include "Instance.h"

// prints-out the vulkan messages to the console
static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT severity,
	VkDebugUtilsMessageTypeFlagsEXT type,
	const VkDebugUtilsMessengerCallbackDataEXT* callback,
	void* data)
{
	//if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
	//{
	//	ENGINE_TRACE("[VERBOSE]: %s", callback->pMessage);
	//	return VK_FALSE;
	//}
	//
	//if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
	//{
	//	ENGINE_TRACE("[INFO]: %s", callback->pMessage);
	//	return VK_FALSE;
	//}

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

	return VK_FALSE;
}

namespace Engine::Renderer
{
	SharedPointer<Instance> Instance::Create(SharedPointer<Window>& window)
	{
		return CreateSharedPointer<Instance>(window);
	}

	Instance::Instance(SharedPointer<Window>& window)
		: m_Window(window)
	{
		ENGINE_TRACE("Creating Instance");

		CreateInstance();
		CreateValidations();
	}

	Instance::~Instance()
	{
		if (m_ValidationsEnabled)
		{
			vkDestroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, nullptr);
		}

		vkDestroyInstance(m_Instance, nullptr);
	}

	bool Instance::CheckValidationSupport()
	{
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		std::set<std::string> requiredLayers(m_Validations.begin(), m_Validations.end());

		for (const auto& layer : availableLayers)
		{
			requiredLayers.erase(layer.layerName);
		}

		return requiredLayers.empty();
	}

	void Instance::CreateInstance()
	{
		VK_CHECK(volkInitialize());

		auto extensions = GetRequiredExtensions(m_Window);

		VkApplicationInfo appci{};
		appci.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appci.pNext = nullptr;
		appci.pApplicationName = "Application";
		appci.applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
		appci.pEngineName = "Cosmos";
		appci.engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);
		appci.apiVersion = VK_MAKE_API_VERSION(0, 1, 0, 0);

		VkDebugUtilsMessengerCreateInfoEXT debugci{};
		debugci.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		debugci.pNext = nullptr;
		debugci.flags = 0;
		debugci.messageSeverity =
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		debugci.messageType =
			VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		debugci.pfnUserCallback = DebugCallback;
		debugci.pUserData = nullptr;

		VkInstanceCreateInfo instci{};
		instci.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		instci.flags = 0;
		instci.pApplicationInfo = &appci;
		instci.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		instci.ppEnabledExtensionNames = extensions.data();

		if (m_ValidationsEnabled)
		{
			instci.enabledLayerCount = static_cast<uint32_t>(m_Validations.size());
			instci.ppEnabledLayerNames = m_Validations.data();
			instci.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugci;
		}

		else
		{
			instci.enabledLayerCount = 0;
			instci.ppEnabledLayerNames = nullptr;
			instci.pNext = nullptr;
		}

		VK_CHECK(vkCreateInstance(&instci, nullptr, &m_Instance));
		volkLoadInstance(m_Instance);
	}

	void Instance::CreateValidations()
	{
		ENGINE_ASSERT((m_ValidationsEnabled && !CheckValidationSupport()) == false, "Validations requested but not available");

		if (!m_ValidationsEnabled) return;

		VkDebugUtilsMessengerCreateInfoEXT ci{};
		ci.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		ci.pNext = nullptr;
		ci.flags = 0;
		ci.messageSeverity =
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		ci.messageType =
			VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
			| VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		ci.pfnUserCallback = DebugCallback;
		ci.pUserData = nullptr;

		VK_CHECK(vkCreateDebugUtilsMessengerEXT(m_Instance, &ci, nullptr, &m_DebugMessenger));
	}

	std::vector<const char*> Instance::GetRequiredExtensions(SharedPointer<Window>& window)
	{
		uint32_t windowExtensionCount;
		const char** windowExtensions = window->GetInstanceExtensions(&windowExtensionCount);
		std::vector<const char*> extensions(windowExtensions, windowExtensions + windowExtensionCount);

		if (m_ValidationsEnabled)
		{
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		return extensions;
	}
}