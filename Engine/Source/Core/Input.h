#pragma once

#include <cstdint>
#include <utility>

#include "Window.h"
#include "Utils/Memory.h"

namespace Engine
{
	class Input
	{
	public:

		// returns a shared pointer to a new input
		static SharedPointer<Input> Create(SharedPointer<Window>& window);

		// constructor
		Input(SharedPointer<Window>& window);

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

		SharedPointer<Window>& m_Window;
	};
}