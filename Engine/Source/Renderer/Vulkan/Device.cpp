#include "enpch.h"
#include "Device.h"

namespace Engine
{
	namespace Vulkan
	{
		void Device::Create(VkInstance& instance, Validations& validations)
		{
			PickPhysicalDevice(instance);
			CreateLogicalDevice(instance, validations);
		}

		void Device::Destroy()
		{
			vkDestroyDevice(m_LogicalDevice, nullptr);
		}

		void Device::PickPhysicalDevice(VkInstance& instance)
		{
			uint32_t deviceCount = 0;
			vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

			std::vector<VkPhysicalDevice> devices(deviceCount);
			vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

			std::multimap<int32_t, VkPhysicalDevice> candidates;

			for (const auto& device : devices)
			{
				int score = RateDeviceSuitability(device);
				candidates.insert(std::make_pair(score, device));
			}

			// at this point all available candidates have been rated and are in order
			m_PhysicalDevice = candidates.rbegin()->second;
		}

		int Device::RateDeviceSuitability(VkPhysicalDevice device)
		{
			VkPhysicalDeviceProperties deviceProperties{};
			vkGetPhysicalDeviceProperties(device, &deviceProperties);

			// discard gpus without graphics queue
			QueueFamilyIndices indices = FindQueueFamilies(device);
			if (indices.graphics.has_value() == 0) return 0;

			int score = 0;

			// favor dedicated gpu
			if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) score += 1000;

			// maximum texture size
			score += deviceProperties.limits.maxImageDimension2D;

			return score;
		}

		QueueFamilyIndices Device::FindQueueFamilies(VkPhysicalDevice device)
		{
			QueueFamilyIndices indices;

			uint32_t queueFamilyCount = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

			std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
			vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

			int i = 0;
			for (const auto& queue : queueFamilies)
			{
				if (queue.queueFlags & VK_QUEUE_GRAPHICS_BIT) // the graphics queue
				{
					indices.graphics = i;
				}

				if (indices.IsComplete()) // all desired queues were found
				{
					break;
				}

				i++;
			}

			return indices;
		}

		void Device::CreateLogicalDevice(VkInstance& instance, Validations& validations)
		{
			VkPhysicalDeviceFeatures deviceFeatures{};

			QueueFamilyIndices indices = FindQueueFamilies(m_PhysicalDevice);
			auto validationlayer = validations.GetValidations();
			float priority = 1.0f;

			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.pNext = nullptr;
			queueCreateInfo.flags = 0;
			queueCreateInfo.queueFamilyIndex = indices.graphics.value();
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &priority;

			VkDeviceCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.queueCreateInfoCount = 1;
			createInfo.pQueueCreateInfos = &queueCreateInfo;
			createInfo.pEnabledFeatures = &deviceFeatures;
			createInfo.enabledExtensionCount = 0;

			// backwards compatibility because before device and instance validations were different before
			if (validations.AreEnabled())
			{
				createInfo.enabledLayerCount = static_cast<uint32_t>(validationlayer.size());
				createInfo.ppEnabledLayerNames = validationlayer.data();
			}

			else
			{
				createInfo.enabledLayerCount = 0;
				createInfo.ppEnabledExtensionNames = nullptr;
			}

			ENGINE_ASSERT(vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_LogicalDevice) == VK_SUCCESS, "Failed to Create Vulkan Device");
			volkLoadDevice(m_LogicalDevice);

			vkGetDeviceQueue(m_LogicalDevice, indices.graphics.value(), 0, &m_GraphicsQueue);
		}
	}
}