#pragma once

#include <volk.h>

#include "Renderer/GraphicsContext.h"

#include "Device.h"
#include "Instance.h"
#include "Validations.h"


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

		public:

			// returns a reference to the instance class
			inline Instance& GetInstance() { return m_Instance; }

			// returns a reference to the validations class
			inline Validations& GetValidations() { return m_Validations; }

			// returns a reference to the device class
			inline Device& GetDevice() { return m_Device; }

		private:

			Window& m_Window;
			Instance m_Instance;
			Validations m_Validations;
			Device m_Device;

		};
	}
}