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
		DrawFrame();
	}

	void VulkanContext::Finish()
	{
		vkDeviceWaitIdle(m_Device->GetNativeVulkanDevice());
	}

	void VulkanContext::DrawFrame()
	{
		vkWaitForFences(m_Device->GetNativeVulkanDevice(), 1, &m_SyncronizationSystem->GetFrameFences()[m_CurrentFrame], VK_TRUE, UINT64_MAX);

		uint32_t imageIndex;
		VkResult result = vkAcquireNextImageKHR(m_Device->GetNativeVulkanDevice(), m_Swapchain->GetNativeVulkanSwapchain(), UINT64_MAX, m_SyncronizationSystem->GetAvailableSemaphores()[m_CurrentFrame], VK_NULL_HANDLE, &imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			RequestSwapchainRecreation();
			return;
		}

		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			ENGINE_ASSERT(false, "Failed to acquire swapchain image");
		}

		vkResetFences(m_Device->GetNativeVulkanDevice(), 1, &m_SyncronizationSystem->GetFrameFences()[m_CurrentFrame]);

		vkResetCommandBuffer(m_GraphicsPipeline->GetCommandBuffer()[m_CurrentFrame], /*VkCommandBufferResetFlagBits*/ 0);
		m_GraphicsPipeline->RecordCommandBuffer(m_GraphicsPipeline->GetCommandBuffer()[m_CurrentFrame], imageIndex);

		VkSemaphore waitSemaphores[] = { m_SyncronizationSystem->GetAvailableSemaphores()[m_CurrentFrame] };
		VkSemaphore signalSemaphores[] = { m_SyncronizationSystem->GetRenderedSemaphores()[m_CurrentFrame] };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.pNext = nullptr;
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_GraphicsPipeline->GetCommandBuffer()[m_CurrentFrame];
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		ENGINE_ASSERT(vkQueueSubmit(m_Device->GetGraphicsQueue(), 1, &submitInfo, m_SyncronizationSystem->GetFrameFences()[m_CurrentFrame]) == VK_SUCCESS, "Failed to submit to graphics queue");

		VkSwapchainKHR swapchains[] = { m_Swapchain->GetNativeVulkanSwapchain() };
		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.pNext = nullptr;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapchains;
		presentInfo.pImageIndices = &imageIndex;
		presentInfo.pResults = nullptr;

		result = vkQueuePresentKHR(m_Device->GetPresentQueue(), &presentInfo);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_Window->WasResized())
		{
			m_Window->SetResized(false);
			RequestSwapchainRecreation();
		}

		else if (result != VK_SUCCESS)
		{
			ENGINE_ASSERT(false, "Failed to present swapchain image");
		}

		m_CurrentFrame = (m_CurrentFrame + 1) % m_Device->GetMaxFramesInFlight();
	}

	void VulkanContext::RequestSwapchainRecreation()
	{
		std::pair<int, int> resolution = m_Window->GetFramebufferSize();
		while (resolution.first == 0 || resolution.second == 0)
		{
			resolution = m_Window->GetFramebufferSize();
			m_Window->WaitEvents();
		}

		vkDeviceWaitIdle(m_Device->GetNativeVulkanDevice());

		m_GraphicsPipeline->CleanFramebuffers();
		m_Swapchain->Cleanup();
		m_Swapchain->CreateSwapchain();
		m_Swapchain->CreateImageViews();
		m_GraphicsPipeline->CreateFramebuffers();
	}
}