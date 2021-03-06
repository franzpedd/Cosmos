#pragma once

#include "Device.h"
#include "Shader.h"
#include "Swapchain.h"
#include "Utils/Memory.h"

#include <volk.h>

namespace Engine::Renderer
{
	class GraphicsPipeline
	{
	public:

		// returns a smart pointer to a new graphics pipeline
		static SharedPointer<GraphicsPipeline> Create(SharedPointer<Device>& device, SharedPointer<Swapchain>& swapchain);

		// constructor
		GraphicsPipeline(SharedPointer<Device>& device, SharedPointer<Swapchain>& swapchain);

		// destructor
		~GraphicsPipeline();

	public:

		// returns the native vulkan command pool
		inline VkCommandPool& GetCommandPool() { return m_CommandPool; }

		// returns the native vulkan command buffer
		inline std::vector<VkCommandBuffer>& GetCommandBuffer() { return m_CommandBuffers; }

	public:

		// creates the render pass
		void CreateRenderPass();

		// creates the shader stage
		void CreateGraphicsPipeline();

		// creates the framebuffers
		void CreateFramebuffers();

		// creates the commandpool
		void CreateCommandPool();

		// creates the commandbuffer
		void CreateCommandBuffer();

	public:

		// records the commandbuffer
		void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

		// cleans current framebuffers
		void CleanFramebuffers();

	private:

		SharedPointer<Device>& m_Device;
		SharedPointer<Swapchain>& m_Swapchain;
		SharedPointer<Shader> m_Vertex;
		SharedPointer<Shader> m_Fragment;

		std::vector<VkDynamicState> m_DynamicStates =
		{
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};

		VkPipelineLayout m_PipelineLayout;
		VkRenderPass m_RenderPass;
		VkPipeline m_Pipeline;

		std::vector<VkFramebuffer> m_SwapchainFramebuffers;

		VkCommandPool m_CommandPool;
		std::vector<VkCommandBuffer> m_CommandBuffers;
	};
}