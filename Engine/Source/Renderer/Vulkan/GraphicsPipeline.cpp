#include "enpch.h"
#include "GraphicsPipeline.h"

namespace Engine::Renderer
{
	SharedPointer<GraphicsPipeline> GraphicsPipeline::Create(SharedPointer<Device>& device, SharedPointer<Swapchain>& swapchain)
	{
		return CreateSharedPointer<GraphicsPipeline>(device, swapchain);
	}

	GraphicsPipeline::GraphicsPipeline(SharedPointer<Device>& device, SharedPointer<Swapchain>& swapchain)
		: m_Device(device), m_Swapchain(swapchain)
	{
		ENGINE_TRACE("Creating Graphics Pipeline");

		// Creates default shaders
		Shader::Specification vertspecs{};
		vertspecs.name = "Simple Vertex Shader";
		vertspecs.path = "Shader/Vulkan/simple_shader.vert";
		vertspecs.type = Shader::Type::Vertex;

		Shader::Specification fragspecs{};
		fragspecs.name = "Simple Fragment Shader";
		fragspecs.path = "Shader/Vulkan/simple_shader.frag";
		fragspecs.type = Shader::Type::Fragment;

		m_Vertex = Shader::Create(m_Device, vertspecs);
		m_Fragment = Shader::Create(m_Device, fragspecs);

		CreateRenderPass();
		CreateGraphicsPipeline();
		CreateFramebuffers();
		CreateCommandPool();
		CreateCommandBuffer();
	}

	GraphicsPipeline::~GraphicsPipeline()
	{
		vkDestroyCommandPool(m_Device->GetNativeVulkanDevice(), m_CommandPool, nullptr);

		for (auto framebuffer : m_SwapchainFramebuffers)
		{
			vkDestroyFramebuffer(m_Device->GetNativeVulkanDevice(), framebuffer, nullptr);
		}

		vkDestroyPipeline(m_Device->GetNativeVulkanDevice(), m_Pipeline, nullptr);
		vkDestroyPipelineLayout(m_Device->GetNativeVulkanDevice(), m_PipelineLayout, nullptr);
		vkDestroyRenderPass(m_Device->GetNativeVulkanDevice(), m_RenderPass, nullptr);
	}

	void GraphicsPipeline::CreateRenderPass()
	{
		VkAttachmentDescription colorAttachment{};
		colorAttachment.flags = 0;
		colorAttachment.format = m_Swapchain->GetSwapchainFormat();
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpassDescription{};
		subpassDescription.flags = 0;
		subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpassDescription.inputAttachmentCount = 0;
		subpassDescription.pInputAttachments = nullptr;
		subpassDescription.colorAttachmentCount = 1;
		subpassDescription.pColorAttachments = &colorAttachmentRef;
		subpassDescription.pResolveAttachments = nullptr;
		subpassDescription.pDepthStencilAttachment = nullptr;
		subpassDescription.preserveAttachmentCount = 0;
		subpassDescription.pPreserveAttachments = nullptr;

		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		dependency.dependencyFlags = 0;

		VkRenderPassCreateInfo renderPassci{};
		renderPassci.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassci.pNext = nullptr;
		renderPassci.flags = 0;
		renderPassci.attachmentCount = 1;
		renderPassci.pAttachments = &colorAttachment;
		renderPassci.subpassCount = 1;
		renderPassci.pSubpasses = &subpassDescription;
		renderPassci.dependencyCount = 1;
		renderPassci.pDependencies = &dependency;

		VkResult res = vkCreateRenderPass(m_Device->GetNativeVulkanDevice(), &renderPassci, nullptr, &m_RenderPass);
		ENGINE_ASSERT(res == VK_SUCCESS, "Failed to create render pass");
	}

	void GraphicsPipeline::CreateGraphicsPipeline()
	{
		VkPipelineShaderStageCreateInfo vertci{};
		vertci.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertci.pNext = nullptr;
		vertci.flags = 0;
		vertci.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertci.module = m_Vertex->GetModule();
		vertci.pName = "main";
		vertci.pSpecializationInfo = nullptr;

		VkPipelineShaderStageCreateInfo fragci{};
		fragci.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragci.pNext = nullptr;
		fragci.flags = 0;
		fragci.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragci.module = m_Fragment->GetModule();
		fragci.pName = "main";
		fragci.pSpecializationInfo = nullptr;

		VkPipelineShaderStageCreateInfo shaderStage[] = { vertci, fragci };

		VkPipelineVertexInputStateCreateInfo vertInputci{};
		vertInputci.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertInputci.pNext = nullptr;
		vertInputci.flags = 0;
		vertInputci.vertexBindingDescriptionCount = 0;
		vertInputci.pVertexBindingDescriptions = nullptr;
		vertInputci.vertexAttributeDescriptionCount = 0;
		vertInputci.pVertexAttributeDescriptions = nullptr;

		VkPipelineInputAssemblyStateCreateInfo inputci{};
		inputci.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputci.pNext = nullptr;
		inputci.flags = 0;
		inputci.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputci.primitiveRestartEnable = VK_FALSE;

		//VkViewport viewport{};
		//viewport.x = 0.0f;
		//viewport.y = 0.0f;
		//viewport.width = (float)m_Device.GetSwapchainExtent2D().width;
		//viewport.height = (float)m_Device.GetSwapchainExtent2D().height;
		//viewport.minDepth = 0.0f;
		//viewport.maxDepth = 1.0f;

		//VkRect2D scissor{};
		//scissor.offset = { 0,0 };
		//scissor.extent = m_Device.GetSwapchainExtent2D();

		VkPipelineDynamicStateCreateInfo dynamicci{};
		dynamicci.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicci.pNext = nullptr;
		dynamicci.flags = 0;
		dynamicci.dynamicStateCount = static_cast<uint32_t>(m_DynamicStates.size());
		dynamicci.pDynamicStates = m_DynamicStates.data();

		VkPipelineViewportStateCreateInfo viewportci{};
		viewportci.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportci.pNext = nullptr;
		viewportci.flags = 0;
		viewportci.viewportCount = 1;
		viewportci.pViewports = nullptr;
		viewportci.scissorCount = 1;
		viewportci.pScissors = nullptr;

		//VkPipelineViewportStateCreateInfo viewportci{};
		//viewportci.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		//viewportci.viewportCount = 1;
		//viewportci.pViewports = &viewport;
		//viewportci.scissorCount = 1;
		//viewportci.pScissors = &scissor;

		VkPipelineRasterizationStateCreateInfo rasterizerci{};
		rasterizerci.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizerci.pNext = nullptr;
		rasterizerci.flags = 0;
		rasterizerci.depthClampEnable = VK_FALSE;
		rasterizerci.rasterizerDiscardEnable = VK_FALSE;
		rasterizerci.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizerci.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizerci.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterizerci.depthBiasEnable = VK_FALSE;
		rasterizerci.depthBiasConstantFactor = 0.0f;
		rasterizerci.depthBiasClamp = 0.0f;
		rasterizerci.depthBiasSlopeFactor = 0.0f;
		rasterizerci.lineWidth = 1.0f;

		VkPipelineMultisampleStateCreateInfo multisamplingci{};
		multisamplingci.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisamplingci.pNext = nullptr;
		multisamplingci.flags = 0;
		multisamplingci.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisamplingci.sampleShadingEnable = VK_FALSE;
		multisamplingci.minSampleShading = 1.0f;
		multisamplingci.pSampleMask = nullptr;
		multisamplingci.alphaToCoverageEnable = VK_FALSE;
		multisamplingci.alphaToOneEnable = VK_FALSE;

		VkPipelineColorBlendAttachmentState colorblendAttachment{};
		colorblendAttachment.blendEnable = VK_FALSE;
		colorblendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
		colorblendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorblendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		colorblendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorblendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorblendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
		colorblendAttachment.colorWriteMask =
			VK_COLOR_COMPONENT_R_BIT |
			VK_COLOR_COMPONENT_G_BIT |
			VK_COLOR_COMPONENT_B_BIT |
			VK_COLOR_COMPONENT_A_BIT;

		VkPipelineColorBlendStateCreateInfo colorblendci{};
		colorblendci.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorblendci.pNext = nullptr;
		colorblendci.flags = 0;
		colorblendci.logicOpEnable = VK_FALSE;
		colorblendci.logicOp = VK_LOGIC_OP_COPY;
		colorblendci.attachmentCount = 1;
		colorblendci.pAttachments = &colorblendAttachment;
		colorblendci.blendConstants[0] = 0.0f;
		colorblendci.blendConstants[1] = 0.0f;
		colorblendci.blendConstants[2] = 0.0f;
		colorblendci.blendConstants[3] = 0.0f;

		VkPipelineLayoutCreateInfo pipelineLayoutci{};
		pipelineLayoutci.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutci.pNext = nullptr;
		pipelineLayoutci.flags = 0;
		pipelineLayoutci.setLayoutCount = 0;
		pipelineLayoutci.pSetLayouts = nullptr;
		pipelineLayoutci.pushConstantRangeCount = 0;
		pipelineLayoutci.pPushConstantRanges = nullptr;

		ENGINE_ASSERT(vkCreatePipelineLayout(m_Device->GetNativeVulkanDevice(), &pipelineLayoutci, nullptr, &m_PipelineLayout) == VK_SUCCESS, "Failed to create vulkan pipeline layout");

		VkGraphicsPipelineCreateInfo gpipelineci{};
		gpipelineci.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		gpipelineci.pNext = nullptr;
		gpipelineci.flags = 0;
		gpipelineci.stageCount = 2;
		gpipelineci.pStages = shaderStage;
		gpipelineci.pVertexInputState = &vertInputci;
		gpipelineci.pInputAssemblyState = &inputci;
		gpipelineci.pTessellationState = nullptr;
		gpipelineci.pViewportState = &viewportci;
		gpipelineci.pRasterizationState = &rasterizerci;
		gpipelineci.pMultisampleState = &multisamplingci;
		gpipelineci.pDepthStencilState = nullptr;
		gpipelineci.pColorBlendState = &colorblendci;
		gpipelineci.pDynamicState = &dynamicci;
		gpipelineci.layout = m_PipelineLayout;
		gpipelineci.renderPass = m_RenderPass;
		gpipelineci.subpass = 0;
		gpipelineci.basePipelineHandle = VK_NULL_HANDLE;
		gpipelineci.basePipelineIndex = -1;

		ENGINE_ASSERT(vkCreateGraphicsPipelines(m_Device->GetNativeVulkanDevice(), VK_NULL_HANDLE, 1, &gpipelineci, nullptr, &m_Pipeline) == VK_SUCCESS, "Failed to create vulkan graphics pipeline");
	}

	void GraphicsPipeline::CreateFramebuffers()
	{
		m_SwapchainFramebuffers.resize(m_Swapchain->GetSwapchainImageViews().size());

		for (size_t i = 0; i < m_Swapchain->GetSwapchainImageViews().size(); i++)
		{
			VkImageView attachments[] = { m_Swapchain->GetSwapchainImageViews()[i] };

			VkFramebufferCreateInfo ci{};
			ci.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			ci.pNext = nullptr;
			ci.flags = 0;
			ci.renderPass = m_RenderPass;
			ci.attachmentCount = 1;
			ci.pAttachments = attachments;
			ci.width = m_Swapchain->GetSwapchainExtent2D().width;
			ci.height = m_Swapchain->GetSwapchainExtent2D().height;
			ci.layers = 1;

			ENGINE_ASSERT(vkCreateFramebuffer(m_Device->GetNativeVulkanDevice(), &ci, nullptr, &m_SwapchainFramebuffers[i]) == VK_SUCCESS, "Failed to create vulkan framebuffers");
		}
	}

	void GraphicsPipeline::CreateCommandPool()
	{
		QueueFamilyIndices queueFamilyIndices = m_Device->QueryFamilyIndices(m_Device->GetNativeVulkanPhysicalDevice());

		VkCommandPoolCreateInfo ci{};
		ci.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		ci.pNext = nullptr;
		ci.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		ci.queueFamilyIndex = queueFamilyIndices.graphics.value();

		ENGINE_ASSERT(vkCreateCommandPool(m_Device->GetNativeVulkanDevice(), &ci, nullptr, &m_CommandPool) == VK_SUCCESS, "Failed to create vulkan command pool");
	}

	void GraphicsPipeline::CreateCommandBuffer()
	{
		m_CommandBuffers.resize(m_Device->GetMaxFramesInFlight());

		VkCommandBufferAllocateInfo alloc{};
		alloc.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		alloc.pNext = nullptr;
		alloc.commandPool = m_CommandPool;
		alloc.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		alloc.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());

		ENGINE_ASSERT(vkAllocateCommandBuffers(m_Device->GetNativeVulkanDevice(), &alloc, m_CommandBuffers.data()) == VK_SUCCESS, "Failed to allocate vulkan command buffers");
	}

	void GraphicsPipeline::RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
	{
		VkCommandBufferBeginInfo commandBegin{};
		commandBegin.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBegin.pNext = nullptr;
		commandBegin.flags = 0;
		commandBegin.pInheritanceInfo = nullptr;

		ENGINE_ASSERT(vkBeginCommandBuffer(commandBuffer, &commandBegin) == VK_SUCCESS, "Failed to begin vulkan command buffers");

		VkClearValue clearColor = { {{0.0f, 0.0f, 0.0f, 1.0f}} };

		VkRenderPassBeginInfo renderpassBegin{};
		renderpassBegin.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderpassBegin.pNext = nullptr;
		renderpassBegin.renderPass = m_RenderPass;
		renderpassBegin.framebuffer = m_SwapchainFramebuffers[imageIndex];
		renderpassBegin.renderArea.offset = { 0,0 };
		renderpassBegin.renderArea.extent = m_Swapchain->GetSwapchainExtent2D();
		renderpassBegin.clearValueCount = 1;
		renderpassBegin.pClearValues = &clearColor;

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast<float>(m_Swapchain->GetSwapchainExtent2D().width);
		viewport.height = static_cast<float>(m_Swapchain->GetSwapchainExtent2D().height);;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = m_Swapchain->GetSwapchainExtent2D();

		vkCmdBeginRenderPass(commandBuffer, &renderpassBegin, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline);

		vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

		vkCmdDraw(commandBuffer, 3, 1, 0, 0);

		vkCmdEndRenderPass(commandBuffer);

		ENGINE_ASSERT(vkEndCommandBuffer(commandBuffer) == VK_SUCCESS, "Failed while ending vulkan command buffer");
	}

	void GraphicsPipeline::CleanFramebuffers()
	{
		for (auto framebuffer : m_SwapchainFramebuffers)
		{
			vkDestroyFramebuffer(m_Device->GetNativeVulkanDevice(), framebuffer, nullptr);
		}
	}
}