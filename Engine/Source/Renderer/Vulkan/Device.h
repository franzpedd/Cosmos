#pragma once

#include <volk.h>

#include "Core/Window.h"

#include "Instance.h"

namespace Engine
{
	namespace Vulkan
	{
		struct QueueFamilyIndices
		{
			std::optional<uint32_t> graphics;
			std::optional<uint32_t> present;

			// returns if all queues were found
			bool IsComplete()
			{
				return graphics.has_value() && present.has_value();
			}
		};

		struct SwapchainSupportDetails
		{
			VkSurfaceCapabilitiesKHR capabilities{};
			std::vector<VkSurfaceFormatKHR> formats;
			std::vector<VkPresentModeKHR> presentModes;
		};

		class Device
		{
		public:

			// returns a pointer to a new device class
			static SharedPointer<Device> Create(Window& window, Vulkan::Instance& instance);

			// constructor
			Device(Window& window, Vulkan::Instance& instance);

			// destructor
			~Device();

		public:

			// returns a reference to the native vulkan physical device
			inline VkPhysicalDevice& GetNativePhysicalDevice() { return m_PhysicalDevice; }

			// returns a reference to the native vulkan device
			inline VkDevice& GetNativeDevice() { return m_Device; }

			// returns a reference to the native vulkan surface
			inline VkSurfaceKHR& GetNativeSurface() { return m_Surface; }

			// returns a reference to the graphics queue
			inline VkQueue& GetGraphicsQueue() { return m_GraphicsQueue; }

			// returns a reference to the present queue
			inline VkQueue& GetPresentQueue() { return m_PresentQueue; }

			// returns a reference for the extensions list
			inline const std::vector<const char*>& GetExtensions() const { m_Extensions; }

		private:

			// creates the vulkan surface
			void CreateSurface();

			// chooses the physical device from all availables
			void PickPhysicalDevice();

			// creates a logical device from the choosen physical device
			void CreateLogicalDevice();

			// creates a swapchain for the device
			void CreateSwapchain();

			// creates the swapchain image views
			void CreateImageViews();

		private:

			// scores a physical device in order to properly choose the best available
			uint32_t RankDevice(VkPhysicalDevice device);

			// populates a queue families by a given device capabilities and properties
			QueueFamilyIndices QueryFamilyIndices(VkPhysicalDevice device);

			// populates a swapchain details by a given device capabilities and properties
			SwapchainSupportDetails QuerySupportDetails(VkPhysicalDevice device);

			// checks if a device supports all required extensions
			bool CheckDeviceExtensionSupport(VkPhysicalDevice device);

			// chooses the best format available from the supported formats 
			VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats);

			// chooses the best present mode from the supported presentmodes
			VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& presentModes);

			// chooses the swap extent/resolution the images from swapchain will be shown
			VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

		private:

			Window& m_Window;
			Instance& m_Instance;

			VkSurfaceKHR m_Surface{ VK_NULL_HANDLE };

			VkPhysicalDevice m_PhysicalDevice{ VK_NULL_HANDLE };
			VkDevice m_Device{ VK_NULL_HANDLE };
			VkQueue m_GraphicsQueue{ VK_NULL_HANDLE };
			VkQueue m_PresentQueue{ VK_NULL_HANDLE };

			const std::vector<const char*> m_Extensions{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };

			VkSwapchainKHR m_Swapchain{ VK_NULL_HANDLE };
			std::vector<VkImage> m_SwapchainImages;
			VkFormat m_SwapchainImageFormat{};
			VkExtent2D m_SwapchainExtent2D{};
			std::vector<VkImageView> m_SwapchainImageViews;

		};
	}
}