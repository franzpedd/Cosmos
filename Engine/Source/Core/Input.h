#pragma once

#include <cstdint>
#include <utility>

#include "Window.h"

namespace Engine
{
	class Input
	{
	public:

		// constructor
		Input(Window& windowref);

		// destructor
		~Input();

	public:

		// updates the keyboard-info
		void Update();

	public:

		// returns either if a given key is being hold at the moment
		uint32_t IsKeyboardKeyDown(uint32_t keycode);

		// returns either if a given mouse button is being hold at the moment
		uint32_t IsMouseButtonDown(uint32_t buttoncode);

		// returns the current mouse position
		std::pair<double, double> GetCurrentMousePosition();

	private:

		Window& m_WindowRef;
	};
}