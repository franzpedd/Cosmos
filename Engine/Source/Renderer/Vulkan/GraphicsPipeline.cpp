#include "enpch.h"
#include "GraphicsPipeline.h"

namespace Engine
{
	namespace Vulkan
	{
		SharedPointer<GraphicsPipeline> GraphicsPipeline::Create(Device& device)
		{
			return CreateSharedPointer<GraphicsPipeline>(device);
		}

		GraphicsPipeline::GraphicsPipeline(Device& device)
			: m_Device(device)
		{
			ENGINE_TRACE("Creating Vulkan Graphics Pipeline");

			CreateVertexShader();
			CreateFragmentShader();
		}

		GraphicsPipeline::~GraphicsPipeline()
		{

		}

		void GraphicsPipeline::CreateVertexShader()
		{
			Shader::Specification specs{};
			specs.name = "Simple Vertex Shader";
			specs.path = "Shader/simple_shader.vert";
			specs.type = Shader::Type::Vertex;

			m_Vertex = Shader::Create(m_Device, specs);
			m_Vertex->CreateShaderModule(m_Device.GetNativeDevice());
		}

		void GraphicsPipeline::CreateFragmentShader()
		{
			Shader::Specification specs;
			specs.name = "Simple Fragment Shader";
			specs.path = "Shader/simple_shader.frag";
			specs.type = Shader::Type::Fragment;

			m_Fragment = Shader::Create(m_Device, specs);
			m_Fragment->CreateShaderModule(m_Device.GetNativeDevice());
		}
	}
}