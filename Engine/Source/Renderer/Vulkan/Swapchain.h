#pragma once

#include "Instance.h"
#include "Surface.h"
#include "Device.h"

#include <volk.h>

namespace Engine::Renderer
{
	class Swapchain
	{
	public:

		// returns a smart pointer to a new swapchain
		static SharedPointer<Swapchain> Create(SharedPointer<Window>& window, SharedPointer<Instance>& instance, SharedPointer<Surface>& surface, SharedPointer<Device>& device);

		// constructor
		Swapchain(SharedPointer<Window>& window, SharedPointer<Instance>& instance, SharedPointer<Surface>& surface, SharedPointer<Device>& device);

		// destructor
		~Swapchain();

		// returns the native vulkan swapchain
		inline VkSwapchainKHR& GetNativeVulkanSwapchain() { return m_Swapchain; }

	public:

		// returns a reference to the swapchain extent
		inline const VkExtent2D& GetSwapchainExtent2D() { return m_SwapchainExtent2D; }

		// returns a referece to the swapchain format
		inline const VkFormat& GetSwapchainFormat() { return m_SwapchainImageFormat; }

		// returns a reference to the swapchain image views
		inline const std::vector<VkImageView>& GetSwapchainImageViews() { return m_SwapchainImageViews; }

	public:

		// creates the swapchain
		void CreateSwapchain();

		// creates the swapchain image views
		void CreateImageViews();

		// cleans the current swapchain
		void Cleanup();

	private:

		// chooses the best format available from the supported formats 
		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats);

		// chooses the best present mode from the supported presentmodes
		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& presentModes);

		// chooses the swap extent/resolution the images from swapchain will be shown
		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

	private:

		SharedPointer<Window>& m_Window;
		SharedPointer<Instance>& m_Instance;
		SharedPointer<Surface>& m_Surface;
		SharedPointer<Device>& m_Device;
		VkSwapchainKHR m_Swapchain = VK_NULL_HANDLE;
		std::vector<VkImage> m_SwapchainImages;
		VkFormat m_SwapchainImageFormat{};
		VkExtent2D m_SwapchainExtent2D{};
		std::vector<VkImageView> m_SwapchainImageViews;
	};
}