#pragma once

#include "Renderer/Context.h"
#include "Device.h"
#include "GraphicsPipeline.h"
#include "Instance.h"
#include "Surface.h"
#include "Swapchain.h"
#include "SyncronizationSystem.h"

namespace Engine::Renderer
{
	class VulkanContext : public Engine::Renderer::Context
	{
	public:

		// constructor
		VulkanContext(SharedPointer<Window>& window);

		// destructor
		virtual ~VulkanContext();

		// returns itself
		virtual inline VulkanContext& GetContext() override { return *this; }

	public:

		// updates the renderer
		virtual void Update() override;

		// stop all pending graphics related functionality
		virtual void Finish() override;

	private:

		// retrieves the next image in the swapchain
		void DrawFrame();

		// requests the swapchain recreation from the swapchain and the graphics pipeline
		void RequestSwapchainRecreation();

	private:

		SharedPointer<Window>& m_Window;
		SharedPointer<Instance> m_Instance;
		SharedPointer<Surface> m_Surface;
		SharedPointer<Device> m_Device;
		SharedPointer<Swapchain> m_Swapchain;
		SharedPointer<GraphicsPipeline> m_GraphicsPipeline;
		SharedPointer<SyncronizationSystem> m_SyncronizationSystem;

		uint32_t m_CurrentFrame = 0;
	};
}