#include "enpch.h"
#include "Device.h"

namespace Engine
{
	namespace Vulkan
	{
		SharedPointer<Device> Device::Create(Window& window, Instance& instance)
		{
			return CreateSharedPointer<Device>(window, instance);
		}

		Device::Device(Window& window, Instance& instance) : m_Window(window), m_Instance(instance)
		{
			ENGINE_TRACE("Creating Vulkan Device");

			CreateSurface();
			PickPhysicalDevice();
			CreateLogicalDevice();
		}

		Device::~Device()
		{
			vkDestroyDevice(m_Device, nullptr);
			vkDestroySurfaceKHR(m_Instance.GetNativeInstance(), m_Surface, nullptr);
		}

		void Device::CreateSurface()
		{
			VkResult res = m_Window.CreateWindowSurface(m_Instance.GetNativeInstance(), m_Surface);
			ENGINE_ASSERT(res == VK_SUCCESS, "Failed to create surface");
		}

		void Device::PickPhysicalDevice()
		{
			uint32_t deviceCount;
			vkEnumeratePhysicalDevices(m_Instance.GetNativeInstance(), &deviceCount, nullptr);

			std::vector<VkPhysicalDevice> devices(deviceCount);
			vkEnumeratePhysicalDevices(m_Instance.GetNativeInstance(), &deviceCount, devices.data());

			std::multimap<uint32_t, VkPhysicalDevice> candidates;

			for (auto& device : devices)
			{
				int score = RankDevice(device);
				candidates.insert(std::make_pair(score, device));
			}

			m_PhysicalDevice = candidates.rbegin()->second;
		}

		void Device::CreateLogicalDevice()
		{
			VkPhysicalDeviceFeatures features{};
			QueueFamilyIndices indices = QueryFamilyIndices(m_PhysicalDevice);

			auto validations = m_Instance.GetValidations();
			float priority = 1.0f;

			std::vector<VkDeviceQueueCreateInfo> queuesci{};
			std::set<uint32_t> uniqueQueues =
			{
				indices.graphics.value(),
				indices.present.value()
			};

			for (uint32_t queue : uniqueQueues)
			{
				VkDeviceQueueCreateInfo qci{};
				qci.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				qci.pNext = nullptr;
				qci.flags = 0;
				qci.queueFamilyIndex = queue;
				qci.queueCount = 1;
				qci.pQueuePriorities = &priority;

				queuesci.push_back(qci);
			}

			VkDeviceCreateInfo ci{};
			ci.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			ci.pNext = nullptr;
			ci.flags = 0;
			ci.queueCreateInfoCount = static_cast<uint32_t>(queuesci.size());
			ci.pQueueCreateInfos = queuesci.data();
			ci.pEnabledFeatures = &features;
			ci.enabledExtensionCount = static_cast<uint32_t>(m_Extensions.size());
			ci.ppEnabledExtensionNames = m_Extensions.data();

			if (m_Instance.AreValidationsEnabled())
			{
				ci.enabledLayerCount = static_cast<uint32_t>(validations.size());
				ci.ppEnabledLayerNames = validations.data();
			}

			else
			{
				ci.enabledLayerCount = 0;
				ci.ppEnabledLayerNames = nullptr;
			}

			VkResult res = vkCreateDevice(m_PhysicalDevice, &ci, nullptr, &m_Device);
			ENGINE_ASSERT(res == VK_SUCCESS, "Failed to create device");

			volkLoadDevice(m_Device);

			vkGetDeviceQueue(m_Device, indices.graphics.value(), 0, &m_GraphicsQueue);
			vkGetDeviceQueue(m_Device, indices.present.value(), 0, &m_PresentQueue);
		}

		uint32_t Device::RankDevice(VkPhysicalDevice device)
		{
			QueueFamilyIndices indices = QueryFamilyIndices(device);
			SwapchainSupportDetails swapchainSupportDetails = QuerySupportDetails(device);
			bool swapchainAdequate = swapchainSupportDetails.formats.empty() && swapchainSupportDetails.presentModes.empty();

			// discarts gpus that doesnt fit our criteria
			if (!indices.IsComplete() || !CheckDeviceExtensionSupport(device) || !swapchainAdequate)
			{
				return 0;
			}

			else
			{
				VkPhysicalDeviceProperties properties{};
				vkGetPhysicalDeviceProperties(device, &properties);

				int score = 0;

				if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
				{
					score += 1000;
				}

				score += properties.limits.maxImageDimension2D;
				return score;
			}

			return 0;
		}

		QueueFamilyIndices Device::QueryFamilyIndices(VkPhysicalDevice device)
		{
			QueueFamilyIndices indices;

			uint32_t queueFamilyCount;
			vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

			std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
			vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

			uint32_t i = 0;
			for (const auto& queue : queueFamilies)
			{
				if (queue.queueFlags & VK_QUEUE_GRAPHICS_BIT) // transfer queue
				{
					indices.graphics = i;
				}

				VkBool32 presentSupport;
				vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_Surface, &presentSupport);

				if (presentSupport) // presentation queue is supported
				{
					indices.present = i;
				}

				if (indices.IsComplete()) // optimize and break out of the loop if all required queues are found
				{
					break;
				}

				i++;
			}

			return indices;
		}

		SwapchainSupportDetails Device::QuerySupportDetails(VkPhysicalDevice device)
		{
			SwapchainSupportDetails details;
			vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_Surface, &details.capabilities);

			uint32_t formatCount;
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, nullptr);

			if (formatCount != 0)
			{
				details.formats.resize(formatCount);
				vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, details.formats.data());
			}

			uint32_t presentModeCount;
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface, &presentModeCount, nullptr);

			if (presentModeCount != 0)
			{
				details.presentModes.resize(presentModeCount);
				vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface, &presentModeCount, details.presentModes.data());
			}

			return details;
		}

		bool Device::CheckDeviceExtensionSupport(VkPhysicalDevice device)
		{
			uint32_t extensionCount;
			vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

			std::vector<VkExtensionProperties> availableExtensions(extensionCount);
			vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

			std::set<std::string> requiredExtensions(m_Extensions.begin(), m_Extensions.end());

			for (const auto& ext : availableExtensions)
			{
				requiredExtensions.erase(ext.extensionName);
			}

			return requiredExtensions.empty();
		}
	}
}