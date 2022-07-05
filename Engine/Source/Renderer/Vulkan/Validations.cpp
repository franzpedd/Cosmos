#include "enpch.h"
#include "Validations.h"

// prints-out the vulkan messages to the console
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

	return VK_FALSE;
}

namespace Engine
{
	namespace Vulkan
	{
		void Validations::Create(VkInstance& instance)
		{
			if (!AreValidationEnabled) return;

			VkDebugUtilsMessengerCreateInfoEXT createInfo{};
			PopulateDebugMessengerCreateInfo(createInfo);

			ENGINE_ASSERT(vkCreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &m_DebugMessenger) == VK_SUCCESS, "Failed to setup debug the messenger");
		}

		void Validations::Destroy(VkInstance& instance)
		{
			if (AreValidationEnabled) vkDestroyDebugUtilsMessengerEXT(instance, m_DebugMessenger, nullptr);
		}

		void Validations::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& object)
		{
			object = {};
			object.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
			object.pNext = nullptr;
			object.flags = 0;
			object.messageSeverity =
				VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
				| VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT
				| VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
				| VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
			object.messageType =
				VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
				| VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
				| VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
			object.pfnUserCallback = DebugCallback;
			object.pUserData = nullptr;
		}

		bool Validations::CheckValidationLayerSupport()
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
	}
}