#include "enpch.h"
#include "VulkanContext.h"

namespace Engine::Renderer
{
	VulkanContext::VulkanContext(SharedPointer<Window>& window)
		: m_Window(window)
	{
		ENGINE_TRACE("Creating Vulkan Context");

		m_Instance = Instance::Create(m_Window);
		m_Surface = Surface::Create(m_Window, m_Instance);
		m_Device = Device::Create(m_Window, m_Instance, m_Surface);
		m_Swapchain = Swapchain::Create(m_Window, m_Instance, m_Surface, m_Device);
		m_GraphicsPipeline = GraphicsPipeline::Create(m_Device, m_Swapchain);
		m_SyncronizationSystem = SyncronizationSystem::Create(m_Device);
	}

	VulkanContext::~VulkanContext()
	{

	}

	void VulkanContext::Update()
	{
		PreSync();
		PreDraw();
	}

	void VulkanContext::Finish()
	{
		vkDeviceWaitIdle(m_Device->GetNativeVulkanDevice());
	}

	void VulkanContext::PreSync()
	{
		m_SyncronizationSystem->WaitForPreviousRender();
	}

	void VulkanContext::PreDraw()
	{
		uint32_t imageIndex;

		vkAcquireNextImageKHR(
			m_Device->GetNativeVulkanDevice(),
			m_Swapchain->GetNativeVulkanSwapchain(),
			UINT64_MAX,
			m_SyncronizationSystem->GetAvailableSemaphore(),
			VK_NULL_HANDLE,
			&imageIndex);

		vkResetCommandBuffer(m_GraphicsPipeline->GetCommandBuffer(), 0);
		m_GraphicsPipeline->RecordCommandBuffer(m_GraphicsPipeline->GetCommandBuffer(), imageIndex);

		VkSemaphore waitSemaphores[] = { m_SyncronizationSystem->GetAvailableSemaphore() };
		VkSemaphore signalSemaphores[] = { m_SyncronizationSystem->GetRenderedSemaphore() };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pNext = nullptr;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_GraphicsPipeline->GetCommandBuffer();
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		VK_CHECK(vkQueueSubmit(m_Device->GetGraphicsQueue(), 1, &submitInfo, m_SyncronizationSystem->GetFrameFence()));

		VkSwapchainKHR swapchains[] = { m_Swapchain->GetNativeVulkanSwapchain() };

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.pNext;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapchains;
		presentInfo.pImageIndices = &imageIndex;
		presentInfo.pResults = nullptr;

		vkQueuePresentKHR(m_Device->GetPresentQueue(), &presentInfo);
	}
}