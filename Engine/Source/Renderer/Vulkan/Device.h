#pragma once

#include <volk.h>

#include "Instance.h"

namespace Engine
{
	namespace Vulkan
	{
		struct QueueFamilyIndices
		{
			// holds the graphics queue
			std::optional<uint32_t> graphics;

			// holds the presentation queue
			std::optional<uint32_t> present;

			// returns true when all queues were found
			bool IsComplete() { return graphics.has_value() && present.has_value(); }
		};

		class Device
		{
		public:

			// constructor
			Device() = default;

			// destructor
			~Device() = default;

			// returns the native vulkan physical device
			inline VkPhysicalDevice& GetNativePhysicalDevice() { return m_PhysicalDevice; }

			// returns the native vulkan logical device
			inline VkDevice& GetNativeLogicalDevice() { return m_LogicalDevice; }

		public:

			// creates the vulkan devices
			void Create(VkInstance& instance, Validations& validations, VkSurfaceKHR& surface);

			// destroys the vulkan devices
			void Destroy();

		private:

			// chooses a physical device between all availables
			void PickPhysicalDevice(VkInstance& instance, VkSurfaceKHR& surface);

			// rates a given device based on requirements
			int RateDeviceSuitability(VkPhysicalDevice device, VkSurfaceKHR& surface);

			// returns the queue families
			QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR& surface);

		private:

			// creates a logical device
			void CreateLogicalDevice(VkInstance& instance, Validations& validations, VkSurfaceKHR& surface);

		private:

			VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
			VkDevice m_LogicalDevice = VK_NULL_HANDLE;
			VkQueue m_GraphicsQueue = VK_NULL_HANDLE;
			VkQueue m_PresentQueue = VK_NULL_HANDLE;
		};
	}
}