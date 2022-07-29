#pragma once

#include "Device.h"
#include "Utils/Memory.h"

#include <volk.h>

namespace Engine::Renderer
{
	class SyncronizationSystem
	{
	public:

		// returns a smart pointer to a new Syncronization System
		static SharedPointer<SyncronizationSystem> Create(SharedPointer<Device>& device);

		// constructor
		SyncronizationSystem(SharedPointer<Device>& device);

		// destructor
		~SyncronizationSystem();

	public:

		// returns the image available semaphore
		inline VkSemaphore& GetAvailableSemaphore() { return m_ImageAvailable; };

		// returns the image finished rendering sempahore
		inline VkSemaphore& GetRenderedSemaphore() { return m_RenderFinished; }

		// returns the frame in flight fence
		inline VkFence& GetFrameFence() { return m_FrameInFlight; }

	public:

		// waits until the last frame is rendered
		void WaitForPreviousRender();

	private:

		SharedPointer<Device>& m_Device;
		VkSemaphore m_ImageAvailable = VK_NULL_HANDLE;	// signals that the image is available
		VkSemaphore m_RenderFinished = VK_NULL_HANDLE;	// signals that the image has finished it's rendering
		VkFence m_FrameInFlight = VK_NULL_HANDLE;		// makes sure only one frame is rendered at a time
	};
}