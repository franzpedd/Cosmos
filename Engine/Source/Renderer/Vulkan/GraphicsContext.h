#pragma once

#include "Renderer/GraphicsContext.h"
#include "Device.h"
#include "GraphicsPipeline.h"
#include "Instance.h"

namespace Engine
{
	namespace Vulkan
	{
		class GraphicsContext : public Engine::GraphicsContext
		{
		public:

			// constructor
			GraphicsContext(Window& window);

			// destructor
			virtual ~GraphicsContext();

		private:

			Window& m_Window;
			SharedPointer<Instance> m_Instance;
			SharedPointer<Device> m_Device;
			SharedPointer<GraphicsPipeline> m_GraphicsPipeline;

		};
	}
}