#pragma once

#include <volk.h>
#include <shaderc/shaderc.hpp>

#include "Device.h"

namespace Engine::Renderer
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
		static SharedPointer<Shader> Create(SharedPointer<Device>& device, Shader::Specification& specs);

		// constructor
		Shader(SharedPointer<Device>& device, Shader::Specification& specs);

		// destructor
		~Shader();

	public:

		// returns the shader's specification
		inline Shader::Specification& GetSpecification() { return m_Specification; }

		// returns the shader's module
		inline VkShaderModule& GetModule() { return m_ShaderModule; }

	private:

		// Creates a shader module based on the specification
		void CreateShaderModule();

		// Compiles to SPIRV-Binary format
		std::vector<uint32_t> Compile();

	private:

		SharedPointer<Device>& m_Device;
		Shader::Specification& m_Specification;
		VkShaderModule m_ShaderModule;
	};
}