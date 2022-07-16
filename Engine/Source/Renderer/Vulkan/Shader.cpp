#include "enpch.h"
#include "Shader.h"

namespace Engine
{
	namespace Vulkan
	{
		SharedPointer<Shader> Shader::Create(Shader::Specification& specs)
		{
			return CreateSharedPointer<Shader>(specs);
		}

		Shader::Shader(Shader::Specification& specs) : m_Specification(specs)
		{
			ENGINE_TRACE("Creating Vulkan Shader");

			WriteBinary();
		}

		Shader::~Shader()
		{

		}

		std::string Shader::ReadSource()
		{
			std::ifstream file(m_Specification.path);

			std::ostringstream buffer{};
			file >> buffer.rdbuf();

			if (file.fail() && !file.eof())
			{
				ENGINE_ASSERT(false, "Failed to open/read %s", m_Specification.path.c_str());
			}

			file.close();

			return buffer.str();
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

		VkShaderModule Shader::CreateShaderModule()
		{
			return VkShaderModule();
		}
	}
}