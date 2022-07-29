#include "enpch.h"
#include "Swapchain.h"

namespace Engine::Renderer
{
	SharedPointer<Swapchain> Swapchain::Create(SharedPointer<Window>& window, SharedPointer<Instance>& instance, SharedPointer<Surface>& surface, SharedPointer<Device>& device)
	{
		return CreateSharedPointer<Swapchain>(window, instance, surface, device);
	}

	Swapchain::Swapchain(SharedPointer<Window>& window, SharedPointer<Instance>& instance, SharedPointer<Surface>& surface, SharedPointer<Device>& device)
		: m_Window(window), m_Instance(instance), m_Surface(surface), m_Device(device)
	{
		ENGINE_TRACE("Creating Swapchain");

		CreateSwapchain();
		CreateImageViews();
	}

	Swapchain::~Swapchain()
	{
		for (auto imageView : m_SwapchainImageViews)
		{
			vkDestroyImageView(m_Device->GetNativeVulkanDevice(), imageView, nullptr);
		}

		vkDestroySwapchainKHR(m_Device->GetNativeVulkanDevice(), m_Swapchain, nullptr);
	}

	void Swapchain::CreateSwapchain()
	{
		QueueFamilyIndices indices = m_Device->QueryFamilyIndices(m_Device->GetNativeVulkanPhysicalDevice());
		SwapchainSupportDetails details = m_Device->QuerySupportDetails(m_Device->GetNativeVulkanPhysicalDevice(), m_Surface->GetNativeVulkanSurface());
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
		ci.surface = m_Surface->GetNativeVulkanSurface();
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

		VK_CHECK(vkCreateSwapchainKHR(m_Device->GetNativeVulkanDevice(), &ci, nullptr, &m_Swapchain));

		vkGetSwapchainImagesKHR(m_Device->GetNativeVulkanDevice(), m_Swapchain, &imageCount, nullptr);
		m_SwapchainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(m_Device->GetNativeVulkanDevice(), m_Swapchain, &imageCount, m_SwapchainImages.data());

		m_SwapchainImageFormat = surfaceFormat.format;
		m_SwapchainExtent2D = extent;
	}

	void Swapchain::CreateImageViews()
	{
		m_SwapchainImageViews.resize(m_SwapchainImages.size());

		for (size_t i = 0; i < m_SwapchainImages.size(); i++)
		{
			VkImageViewCreateInfo ci{};
			ci.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			ci.pNext = nullptr;
			ci.flags = 0;
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

			VK_CHECK(vkCreateImageView(m_Device->GetNativeVulkanDevice(), &ci, nullptr, &m_SwapchainImageViews[i]));
		}
	}

	VkSurfaceFormatKHR Swapchain::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats)
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

	VkPresentModeKHR Swapchain::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& presentModes)
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

	VkExtent2D Swapchain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
	{
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		{
			return capabilities.currentExtent;
		}

		else
		{
			std::pair<int, int> resolution = m_Window->GetFramebufferSize();
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