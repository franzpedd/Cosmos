#include "enpch.h"
#include "GraphicsPipeline.h"

namespace Engine
{
	namespace Vulkan
	{
		SharedPointer<GraphicsPipeline> GraphicsPipeline::Create()
		{
			return CreateSharedPointer<GraphicsPipeline>();
		}

		GraphicsPipeline::GraphicsPipeline()
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

			m_Vertex = Shader::Create(specs);
		}

		void GraphicsPipeline::CreateFragmentShader()
		{
			Shader::Specification specs;
			specs.name = "Simple Fragment Shader";
			specs.path = "Shader/simple_shader.frag";
			specs.type = Shader::Type::Fragment;

			m_Fragment = Shader::Create(specs);
		}
	}
}