#include "enpch.h"
#include "Shader.h"

namespace Engine
{
	namespace Vulkan
	{
		SharedPointer<Shader> Shader::Create(Device& device, Shader::Specification& specs)
		{
			return CreateSharedPointer<Shader>(device, specs);
		}

		Shader::Shader(Device& device, Shader::Specification& specs) : m_Specification(specs), m_Device(device)
		{
			ENGINE_TRACE("Creating Vulkan Shader");

			WriteBinary();
		}

		Shader::~Shader()
		{
			vkDestroyShaderModule(m_Device.GetNativeDevice(), m_ShaderModule, nullptr);
		}

		VkShaderModule Shader::CreateShaderModule(VkDevice device)
		{
			std::vector<char> code = ReadBinary();

			VkShaderModuleCreateInfo ci{};
			ci.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			ci.pNext = nullptr;
			ci.flags = 0;
			ci.codeSize = code.size();
			ci.pCode = reinterpret_cast<const uint32_t*>(code.data());

			VkResult res = vkCreateShaderModule(device, &ci, nullptr, &m_ShaderModule);
			ENGINE_ASSERT(res == VK_SUCCESS, "Failed to create shader module");

			return m_ShaderModule;
		}

		std::string Shader::ReadSource()
		{
			std::ifstream file(m_Specification.path);

			if (!file.is_open())
			{
				ENGINE_ASSERT(false, "Failed to open %s", m_Specification.path);
			}

			std::ostringstream buffer{};
			file >> buffer.rdbuf();

			file.close();

			return buffer.str();
		}

		std::vector<char> Shader::ReadBinary()
		{
			std::ifstream file(m_Specification.path + ".spv", std::ios::ate | std::ios::binary);

			if (!file.is_open())
			{
				ENGINE_ASSERT(false, "Failed to open %s", m_Specification.path);
			}

			size_t fileSize = (size_t)file.tellg();
			std::vector<char> buffer(fileSize);

			file.seekg(0);
			file.read(buffer.data(), fileSize);
			file.close();

			return buffer;
		}

		void Shader::WriteBinary()
		{
			std::vector<uint32_t> bytecode = Compile();
			std::ofstream file(m_Specification.path + ".spv", std::ios::out | std::ios::binary);

			if (!bytecode.empty())
			{
				file.write(reinterpret_cast<char*>(&bytecode[0]), bytecode.size() * sizeof(bytecode[0]));
			}

			file.close();
		}

		std::vector<uint32_t> Shader::Compile()
		{
			std::string source = ReadSource();
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
}