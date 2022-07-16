#pragma once

#include <volk.h>

#include "Device.h"
#include "Shader.h"
#include "Util/Memory.h"

namespace Engine
{
	namespace Vulkan
	{
		class GraphicsPipeline
		{
		public:

			// returns a pointer to a new instance class
			static SharedPointer<GraphicsPipeline> Create(Device& device);

			// constructor
			GraphicsPipeline(Device& device);

			// destructor
			~GraphicsPipeline();

		public:

		private:

			// creates the vertex shader class
			void CreateVertexShader();

			// creates the fragment shader class
			void CreateFragmentShader();

		private:

			Device& m_Device;
			SharedPointer<Shader> m_Vertex;
			SharedPointer<Shader> m_Fragment;

		};
	}
}