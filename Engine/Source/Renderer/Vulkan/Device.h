#pragma once

#include "Instance.h"
#include "Surface.h"

#include <volk.h>

namespace Engine::Renderer
{
	struct SwapchainSupportDetails
	{
		VkSurfaceCapabilitiesKHR capabilities{};
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

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

	class Device
	{
	public:

		// returns a smart pointer to a new device
		static SharedPointer<Device> Create(SharedPointer<Window>& window, SharedPointer<Instance>& instance, SharedPointer<Surface>& surface);

		// constructor
		Device(SharedPointer<Window>& window, SharedPointer<Instance>& instance, SharedPointer<Surface>& surface);

		// destructor
		~Device();

		// returns the native vulkan device
		inline VkDevice& GetNativeVulkanDevice() { return m_Device; }

		// returns the native vulkan physical device
		inline VkPhysicalDevice& GetNativeVulkanPhysicalDevice() { return m_PhysicalDevice; }

	public:

		// returns a reference to the graphics queue
		inline VkQueue& GetGraphicsQueue() { return m_GraphicsQueue; }

		// returns a reference to the present queue
		inline VkQueue& GetPresentQueue() { return m_PresentQueue; }

		// returns a reference for the extensions list
		inline const std::vector<const char*>& GetExtensions() const { m_Extensions; }

		// returns the max quantity of frames that can be asynchronously processed at a given time
		inline const int GetMaxFramesInFlight() { return m_MaxFramesInFlight; }

	public:

		// populates a swapchain details by a given device capabilities and properties
		SwapchainSupportDetails QuerySupportDetails(VkPhysicalDevice device, VkSurfaceKHR surface);

		// populates a queue families by a given device capabilities and properties
		QueueFamilyIndices QueryFamilyIndices(VkPhysicalDevice device);

		// checks if a device supports all required extensions
		bool CheckDeviceExtensionSupport(VkPhysicalDevice device);

	private:

		// chooses the physical device from all availables
		void PickPhysicalDevice();

		// creates a logical device from the choosen physical device
		void CreateLogicalDevice();

		// scores a physical device in order to properly choose the best available
		uint32_t RankDevice(VkPhysicalDevice device);

	private:

		SharedPointer<Window>& m_Window;
		SharedPointer<Instance>& m_Instance;
		SharedPointer<Surface>& m_Surface;
		VkDevice m_Device = VK_NULL_HANDLE;
		VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
		VkQueue m_GraphicsQueue = VK_NULL_HANDLE;
		VkQueue m_PresentQueue = VK_NULL_HANDLE;
		const std::vector<const char*> m_Extensions{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };
		const int m_MaxFramesInFlight = 2; // increase may cause frames of lattency
	};
}