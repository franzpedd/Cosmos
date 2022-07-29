#include "enpch.h"
#include "SyncronizationSystem.h"

namespace Engine::Renderer
{
	SharedPointer<SyncronizationSystem> SyncronizationSystem::Create(SharedPointer<Device>& device)
	{
		return CreateSharedPointer<SyncronizationSystem>(device);
	}

	SyncronizationSystem::SyncronizationSystem(SharedPointer<Device>& device)
		: m_Device(device)
	{
		ENGINE_TRACE("Creating Syncronization System");

		VkSemaphoreCreateInfo semaphoreci{};
		semaphoreci.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		semaphoreci.flags = 0;
		semaphoreci.pNext = nullptr;

		VkFenceCreateInfo fenceci{};
		fenceci.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceci.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		fenceci.pNext = nullptr;

		VK_CHECK(vkCreateSemaphore(m_Device->GetNativeVulkanDevice(), &semaphoreci, nullptr, &m_ImageAvailable));
		VK_CHECK(vkCreateSemaphore(m_Device->GetNativeVulkanDevice(), &semaphoreci, nullptr, &m_RenderFinished));
		VK_CHECK(vkCreateFence(m_Device->GetNativeVulkanDevice(), &fenceci, nullptr, &m_FrameInFlight));
	}

	SyncronizationSystem::~SyncronizationSystem()
	{
		vkDestroySemaphore(m_Device->GetNativeVulkanDevice(), m_ImageAvailable, nullptr);
		vkDestroySemaphore(m_Device->GetNativeVulkanDevice(), m_RenderFinished, nullptr);
		vkDestroyFence(m_Device->GetNativeVulkanDevice(), m_FrameInFlight, nullptr);
	}

	void SyncronizationSystem::WaitForPreviousRender()
	{
		vkWaitForFences(m_Device->GetNativeVulkanDevice(), 1, &m_FrameInFlight, VK_TRUE, UINT64_MAX);
		vkResetFences(m_Device->GetNativeVulkanDevice(), 1, &m_FrameInFlight);
	}
}