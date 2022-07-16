#pragma once

#include <volk.h>
#include <shaderc/shaderc.hpp>

#include "Device.h"

namespace Engine
{
	namespace Vulkan
	{
		class Shader
		{
		public:

			enum class Type
			{
				Unsigned = -1,
				Vertex,
				Fragment,
				Compute,
				Geometry,
				TesselationControll,
				TesselationEvaluation
			};

			struct Specification
			{
				std::string name;
				std::string path;
				Type type = Type::Unsigned;
			};

		public:

			// returns a pointer to a new shader class
			static SharedPointer<Shader> Create(Device& device, Shader::Specification& specs);

			// constructor
			Shader(Device& device, Shader::Specification& specs);

			// destructor
			~Shader();

		public:

			// returns the shader's name
			inline std::string& GetName() { return m_Specification.name; }

			// returns the shader's module
			inline VkShaderModule& GetModule() { return m_ShaderModule; }

			// Creates a shader module based on the specification
			VkShaderModule CreateShaderModule(VkDevice device);

		private:

			// reads the shader's source file
			std::string ReadSource();

			// reads the shader's binary file
			std::vector<char> ReadBinary();

			// Writes a compiled SPIRV binary to a file on disk with the shader's name + ".spv"
			void WriteBinary();

			// Compiles to SPIRV-Binary format
			std::vector<uint32_t> Compile();

		private:

			Device& m_Device;
			Shader::Specification& m_Specification;
			VkShaderModule m_ShaderModule;
		};
	}
}