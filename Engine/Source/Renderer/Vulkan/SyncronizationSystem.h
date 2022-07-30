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
		inline std::vector<VkSemaphore>& GetAvailableSemaphores() { return m_ImageAvailableSemaphores; };

		// returns the image finished rendering sempahore
		inline std::vector<VkSemaphore>& GetRenderedSemaphores() { return m_FinishedRendererSempahores; }

		// returns the frame in flight fence
		inline std::vector<VkFence>& GetFrameFences() { return m_InFlightFences; }

	private:

		SharedPointer<Device>& m_Device;
		std::vector<VkSemaphore> m_ImageAvailableSemaphores;	// signals that the image is available
		std::vector<VkSemaphore> m_FinishedRendererSempahores;	// signals that the image has finished it's rendering
		std::vector<VkFence> m_InFlightFences;					// makes sure only one frame is rendered at a time

	};
}