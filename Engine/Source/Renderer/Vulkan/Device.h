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

			// returns true when all queues were found
			bool IsComplete() { return graphics.has_value(); }
		};

		class Device
		{
		public:

			// constructor
			Device() = default;

			// destructor
			~Device() = default;

		public:

			// creates the vulkan devices
			void Create(VkInstance& instance, Validations& validations);

			// destroys the vulkan devices
			void Destroy();

		private:

			// chooses a physical device between all availables
			void PickPhysicalDevice(VkInstance& instance);

			// rates a given device based on requirements
			int RateDeviceSuitability(VkPhysicalDevice device);

			// returns the queue families
			QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);

		private:

			// creates a logical device
			void CreateLogicalDevice(VkInstance& instance, Validations& validations);

		private:

			VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
			VkDevice m_LogicalDevice = VK_NULL_HANDLE;
			VkQueue m_GraphicsQueue = VK_NULL_HANDLE;
		};
	}
}