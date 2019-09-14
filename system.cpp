#include "system.h"

#include "program.h"
#include "program_select.h"

#include "SDL_ttf.h"

#include <cassert>

namespace
{
	constexpr int kPixelWidth = 10;
	constexpr int kPixelHeight = 10;

	// chip8 uses a 64x32 screen
	constexpr int kNumPixelsWide = 64;
	constexpr int kNumPixelsHigh = 32;
}

namespace chip8
{
	System::System()
	{
		int result = SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS);
		assert(result == 0);
		result = TTF_Init();
		assert(result == 0);

		result = SDL_CreateWindowAndRenderer(kPixelWidth * kNumPixelsWide, kPixelHeight * kNumPixelsHigh,
			SDL_WINDOW_BORDERLESS | SDL_WINDOW_SHOWN,
			&mWindow,
			&mRenderer);
		assert(result == 0);

		// Start with the program selection prompt
		mProcess = std::make_unique<ProgramSelect>();
	}

	System::~System()
	{
		SDL_DestroyRenderer(mRenderer);
		SDL_DestroyWindow(mWindow);

		TTF_Quit();
		SDL_Quit();
	}

	void System::Run()
	{
		bool quit = false;
		while (!quit)
		{
			// Process any system events
			SDL_Event event;
			while (!quit && SDL_PollEvent(&event))
			{
				switch (event.type)
				{
				case SDL_KEYDOWN:
					mProcess->OnKeyDown(event.key.keysym);
					break;
				case SDL_KEYUP:
					mProcess->OnKeyUp(event.key.keysym);
					break;
				case SDL_QUIT:
					quit = true;
					break;
				}
			}

			// Update the process a little
			mProcess->Render(mRenderer);

			// Blit to screen
			SDL_RenderPresent(mRenderer);

			// Check if the process want to switch out
			if (mProcess->Finished())
				mProcess = mProcess->NextProcess();

			// If there's no process, return to the program select
			if (mProcess == nullptr)
				mProcess = std::make_unique<ProgramSelect>();
		}
	}
}