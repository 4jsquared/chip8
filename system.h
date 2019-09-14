#ifndef CHIP8_SYSTEM_H
#define CHIP8_SYSTEM_H

#include "process.h"

#include "SDL.h"

#include <memory>

namespace chip8
{
	class System
	{
	public:
		System();
		~System();

		void Run();

	private:
		std::unique_ptr<Process> mProcess;

		SDL_Window* mWindow;
		SDL_Renderer* mRenderer;
	};
}

#endif // CHIP8_SYSTEM_H