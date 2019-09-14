#ifndef CHIP8_PROCESS_H
#define CHIP8_PROCESS_H

#include "SDL.h"

#include <memory>

namespace chip8
{
	class Process
	{
	public:
		virtual ~Process() {}

		virtual void Render(SDL_Renderer* renderer) = 0;
		virtual bool Finished() = 0;

		virtual std::unique_ptr<Process> NextProcess() { return nullptr; }

		virtual void OnKeyDown(const SDL_Keysym& keysym) {}
		virtual void OnKeyUp(const SDL_Keysym& keysym) {}
	};
}

#endif // CHIP8_PROCESS_H