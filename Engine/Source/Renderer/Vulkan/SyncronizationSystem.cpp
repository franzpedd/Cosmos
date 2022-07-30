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

		m_ImageAvailableSemaphores.resize(m_Device->GetMaxFramesInFlight());
		m_FinishedRendererSempahores.resize(m_Device->GetMaxFramesInFlight());
		m_InFlightFences.resize(m_Device->GetMaxFramesInFlight());

		VkSemaphoreCreateInfo semaphoreci{};
		semaphoreci.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		semaphoreci.flags = 0;
		semaphoreci.pNext = nullptr;

		VkFenceCreateInfo fenceci{};
		fenceci.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceci.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		fenceci.pNext = nullptr;

		for (int i = 0; i < m_Device->GetMaxFramesInFlight(); i++)
		{
			VK_CHECK(vkCreateSemaphore(m_Device->GetNativeVulkanDevice(), &semaphoreci, nullptr, &m_ImageAvailableSemaphores[i]));
			VK_CHECK(vkCreateSemaphore(m_Device->GetNativeVulkanDevice(), &semaphoreci, nullptr, &m_FinishedRendererSempahores[i]));
			VK_CHECK(vkCreateFence(m_Device->GetNativeVulkanDevice(), &fenceci, nullptr, &m_InFlightFences[i]));
		}
	}

	SyncronizationSystem::~SyncronizationSystem()
	{
		for (int i = 0; i < m_Device->GetMaxFramesInFlight(); i++)
		{
			vkDestroySemaphore(m_Device->GetNativeVulkanDevice(), m_ImageAvailableSemaphores[i], nullptr);
			vkDestroySemaphore(m_Device->GetNativeVulkanDevice(), m_FinishedRendererSempahores[i], nullptr);
			vkDestroyFence(m_Device->GetNativeVulkanDevice(), m_InFlightFences[i], nullptr);
		}
	}
}