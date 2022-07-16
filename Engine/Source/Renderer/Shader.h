#pragma once

#include "GraphicsContext.h"
#include "Util/Memory.h"

namespace Engine
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

		// creates a shader object
		static SharedPointer<Shader> Create(Specification& specs, RendererApi api = RendererApi::VULKAN);

	};
}