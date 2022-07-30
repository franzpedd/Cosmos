#include "enpch.h"
#include "Shader.h"

namespace Engine::Renderer
{
	SharedPointer<Shader> Shader::Create(SharedPointer<Device>& device, Shader::Specification& specs)
	{
		return CreateSharedPointer<Shader>(device, specs);
	}

	Shader::Shader(SharedPointer<Device>& device, Shader::Specification& specs)
		: m_Specification(specs), m_Device(device)
	{
		ENGINE_TRACE("Creating Shader for %s", specs.path.c_str());

		FileSystem::WriteBinaryToFile(m_Specification.path + ".spv", Compile());
		CreateShaderModule();
	}

	Shader::~Shader()
	{
		vkDestroyShaderModule(m_Device->GetNativeVulkanDevice(), m_ShaderModule, nullptr);
	}

	void Shader::CreateShaderModule()
	{
		std::vector<char> code = FileSystem::ReadBinaryFromFile(m_Specification.path + ".spv");

		VkShaderModuleCreateInfo ci{};
		ci.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		ci.pNext = nullptr;
		ci.flags = 0;
		ci.codeSize = code.size();
		ci.pCode = reinterpret_cast<const uint32_t*>(code.data());

		ENGINE_ASSERT(vkCreateShaderModule(m_Device->GetNativeVulkanDevice(), &ci, nullptr, &m_ShaderModule) == VK_SUCCESS, "Failed to create vulkan shader module");
	}

	std::vector<uint32_t> Shader::Compile()
	{
		std::string source = FileSystem::ReadTextFromFile(m_Specification.path);
		shaderc::Compiler compiler;
		shaderc::CompileOptions options;

		shaderc::SpvCompilationResult result = compiler.CompileGlslToSpv(
			source.c_str(),
			source.size(),
			static_cast<shaderc_shader_kind>(m_Specification.type),
			m_Specification.path.c_str(),
			options);

		ENGINE_ASSERT(result.GetCompilationStatus() == shaderc_compilation_status_success, "%s", result.GetErrorMessage().c_str());

		return { result.cbegin(), result.cend() };
	}
}