#include "enpch.h"
#include "Device.h"

namespace Engine
{
	namespace Vulkan
	{
		void Device::Create(VkInstance& instance, Validations& validations, VkSurfaceKHR& surface)
		{
			PickPhysicalDevice(instance, surface);
			CreateLogicalDevice(instance, validations, surface);
		}

		void Device::Destroy()
		{
			vkDestroyDevice(m_LogicalDevice, nullptr);
		}

		void Device::PickPhysicalDevice(VkInstance& instance, VkSurfaceKHR& surface)
		{
			uint32_t deviceCount = 0;
			vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

			std::vector<VkPhysicalDevice> devices(deviceCount);
			vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

			std::multimap<int32_t, VkPhysicalDevice> candidates;

			for (const auto& device : devices)
			{
				int score = RateDeviceSuitability(device, surface);
				candidates.insert(std::make_pair(score, device));
			}

			// at this point all available candidates have been rated and are in order
			m_PhysicalDevice = candidates.rbegin()->second;
		}

		int Device::RateDeviceSuitability(VkPhysicalDevice device, VkSurfaceKHR& surface)
		{
			VkPhysicalDeviceProperties deviceProperties{};
			vkGetPhysicalDeviceProperties(device, &deviceProperties);

			// discard gpus without graphics queue
			QueueFamilyIndices indices = FindQueueFamilies(device, surface);
			if (indices.graphics.has_value() == 0) return 0;

			int score = 0;

			// favor dedicated gpu
			if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) score += 1000;

			// maximum texture size
			score += deviceProperties.limits.maxImageDimension2D;

			return score;
		}

		QueueFamilyIndices Device::FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR& surface)
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

				VkBool32 presentSupport;
				vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

				if (presentSupport) // the presentation queue
				{
					indices.present = i;
				}

				if (indices.IsComplete()) // all desired queues were found
				{
					break;
				}

				i++;
			}

			return indices;
		}

		void Device::CreateLogicalDevice(VkInstance& instance, Validations& validations, VkSurfaceKHR& surface)
		{
			VkPhysicalDeviceFeatures deviceFeatures{};

			QueueFamilyIndices indices = FindQueueFamilies(m_PhysicalDevice, surface);
			auto validationlayer = validations.GetValidations();
			float priority = 1.0f;

			// list of all unique queues
			std::set<uint32_t> uniqueQueues =
			{
				indices.graphics.value(),
				indices.present.value()
			};

			// populate createinfo for all queues
			std::vector<VkDeviceQueueCreateInfo> queuesCreateInfos{};
			for (uint32_t queue : uniqueQueues)
			{
				VkDeviceQueueCreateInfo queueCreateInfo{};
				queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				queueCreateInfo.pNext = nullptr;
				queueCreateInfo.flags = 0;
				queueCreateInfo.queueFamilyIndex = queue;
				queueCreateInfo.queueCount = 1;
				queueCreateInfo.pQueuePriorities = &priority;

				queuesCreateInfos.push_back(queueCreateInfo);
			}

			// populate device create info
			VkDeviceCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.queueCreateInfoCount = static_cast<uint32_t>(queuesCreateInfos.size());
			createInfo.pQueueCreateInfos = queuesCreateInfos.data();
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
			vkGetDeviceQueue(m_LogicalDevice, indices.present.value(), 0, &m_PresentQueue);
		}
	}
}