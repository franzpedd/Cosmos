#pragma once

#include "Window.h"
#include "Renderer/GraphicsContext.h"

namespace Engine
{
	class Renderer
	{
	public:

		// constructor
		Renderer(Window& windowref);

		// destructor
		~Renderer();

	public:

		// updates the renderer
		void Update();

	private:

		Window& m_WindowRef;
		UniquePointer<GraphicsContext> m_GraphicsContext;

	};
}