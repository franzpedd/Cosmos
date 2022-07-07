#pragma once

#include <volk.h>

#include "Core/Window.h"

namespace Engine
{
	namespace Vulkan
	{
		class Surface
		{
		public:

			// constructor
			Surface() = default;

			// destructor
			~Surface() = default;

			// returns the native vulkan surface
			inline VkSurfaceKHR& GetNativeSurface() { return m_Surface; }

		public:

			// creates the surface
			void Create(Window& window, VkInstance& instance);

			// destroys the surface
			void Destroy(VkInstance& instance);

		private:

			VkSurfaceKHR m_Surface;
		};
	}
}