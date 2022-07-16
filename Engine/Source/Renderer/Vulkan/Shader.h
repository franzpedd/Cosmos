#pragma once

#include <shaderc/shaderc.hpp>

#include "Renderer/Shader.h"

namespace Engine
{
	namespace Vulkan
	{
		class Shader : public Engine::Shader
		{
		public:

			// returns a pointer to a new shader class
			static SharedPointer<Shader> Create(Shader::Specification& specs);

			// constructor
			Shader(Shader::Specification& specs);

			// destructor
			~Shader();

		public:

			// returns the shader's name
			inline std::string& GetName() { return m_Specification.name; }

		private:

			// reads a text/source file given it's path
			std::string ReadSource();

			// Writes a compiled SPIRV binary to a file on disk with the shader's name + ".spv"
			void WriteBinary();

			// Compiles to SPIRV-Binary format
			std::vector<uint32_t> Compile();

			// Creates a shader module based on the specification
			VkShaderModule CreateShaderModule();

		private:

			Shader::Specification& m_Specification;

		};
	}
}