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
			CreateSwapchain();
			CreateImageViews();
		}

		Device::~Device()
		{
			for (auto imageView : m_SwapchainImageViews)
			{
				vkDestroyImageView(m_Device, imageView, nullptr);
			}

			vkDestroySwapchainKHR(m_Device, m_Swapchain, nullptr);
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

		void Device::CreateSwapchain()
		{
			QueueFamilyIndices indices = QueryFamilyIndices(m_PhysicalDevice);
			SwapchainSupportDetails details = QuerySupportDetails(m_PhysicalDevice);
			VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(details.formats);
			VkPresentModeKHR presentMode = ChooseSwapPresentMode(details.presentModes);
			VkExtent2D extent = ChooseSwapExtent(details.capabilities);

			uint32_t queueFamilyIndices[] = { indices.graphics.value(), indices.present.value() };
			uint32_t imageCount = details.capabilities.minImageCount + 1; // request one more than the minumum to avoid waiting

			// avoid exceeding maximum images number, 0 means there is not maximum
			if (details.capabilities.maxImageCount > 0 && imageCount > details.capabilities.maxImageCount)
			{
				imageCount = details.capabilities.maxImageCount;
			}

			VkSwapchainCreateInfoKHR ci{};
			ci.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			ci.pNext = nullptr;
			ci.flags = 0;
			ci.surface = m_Surface;
			ci.minImageCount = imageCount;
			ci.imageFormat = surfaceFormat.format;
			ci.imageColorSpace = surfaceFormat.colorSpace;
			ci.imageExtent = extent;
			ci.imageArrayLayers = 1;
			ci.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

			if (indices.graphics != indices.present)
			{
				ci.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
				ci.queueFamilyIndexCount = 2;
				ci.pQueueFamilyIndices = queueFamilyIndices;
			}

			else
			{
				ci.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
				ci.queueFamilyIndexCount = 0;
				ci.pQueueFamilyIndices = nullptr;
			}

			ci.preTransform = details.capabilities.currentTransform;
			ci.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
			ci.presentMode = presentMode;
			ci.clipped = VK_TRUE;
			ci.oldSwapchain = VK_NULL_HANDLE;

			VkResult res = vkCreateSwapchainKHR(m_Device, &ci, nullptr, &m_Swapchain);
			ENGINE_ASSERT(res == VK_SUCCESS, "Failed to create swapchain");

			vkGetSwapchainImagesKHR(m_Device, m_Swapchain, &imageCount, nullptr);
			m_SwapchainImages.resize(imageCount);
			vkGetSwapchainImagesKHR(m_Device, m_Swapchain, &imageCount, m_SwapchainImages.data());

			m_SwapchainImageFormat = surfaceFormat.format;
			m_SwapchainExtent2D = extent;
		}

		void Device::CreateImageViews()
		{
			m_SwapchainImageViews.resize(m_SwapchainImages.size());

			for (size_t i = 0; i < m_SwapchainImages.size(); i++)
			{
				VkImageViewCreateInfo ci{};
				ci.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
				//ci.pNext = nullptr;
				//ci.flags = 0;
				ci.image = m_SwapchainImages[i];
				ci.viewType = VK_IMAGE_VIEW_TYPE_2D;
				ci.format = m_SwapchainImageFormat;
				ci.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
				ci.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
				ci.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
				ci.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
				ci.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				ci.subresourceRange.baseMipLevel = 0;
				ci.subresourceRange.levelCount = 1;
				ci.subresourceRange.baseArrayLayer = 0;
				ci.subresourceRange.layerCount = 1;

				VkResult res = vkCreateImageView(m_Device, &ci, nullptr, &m_SwapchainImageViews[i]);
				ENGINE_ASSERT(res == VK_SUCCESS, "Failed to create image view");
			}
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

		VkSurfaceFormatKHR Device::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats)
		{
			for (const auto& availableFormat : formats)
			{
				if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				{
					return availableFormat;
				}
			}

			return formats[0];
		}

		VkPresentModeKHR Device::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& presentModes)
		{
			for (const auto& availablePresentMode : presentModes)
			{
				if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
				{
					return availablePresentMode;
				}
			}

			return VK_PRESENT_MODE_FIFO_KHR;
		}

		VkExtent2D Device::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
		{
			if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
			{
				return capabilities.currentExtent;
			}

			else
			{
				std::pair<int, int> resolution = m_Window.GetFramebufferSize();
				VkExtent2D extent =
				{
					static_cast<uint32_t>(resolution.first),
					static_cast<uint32_t>(resolution.second)
				};

				extent.width = std::clamp(extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
				extent.height = std::clamp(extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

				return extent;
			}
		}
	}
}