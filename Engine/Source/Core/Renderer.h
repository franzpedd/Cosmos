#pragma once

#include "Window.h"
#include "Renderer/GraphicsContext.h"

namespace Engine
{
	class Renderer
	{
	public:

		// constructor
		Renderer(Window& window);

		// destructor
		~Renderer();

	public:

		// updates the renderer
		void Update();

	private:

		Window& m_Window;
		SharedPointer<GraphicsContext> m_GraphicsContext;

	};
}