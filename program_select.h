#ifndef CHIP8_PROGRAM_SELECT_H
#define CHIP8_PROGRAM_SELECT_H

#include "process.h"

#include "SDL_ttf.h"

#include <filesystem>

namespace chip8
{
	class ProgramSelect : public Process
	{
	public:
		ProgramSelect();
		~ProgramSelect();

		void Render(SDL_Renderer* renderer) override;
		bool Finished() override;

		std::unique_ptr<Process> NextProcess() override;

		void OnKeyDown(const SDL_Keysym& keysym) override {};
		void OnKeyUp(const SDL_Keysym& keysym) override;

	private:
		void UpdatePaths();

		SDL_Surface* CreateTextSurface(const std::filesystem::path& path, const SDL_Color& color);
		void RenderSurface(SDL_Renderer* renderer, SDL_Surface* surface, int pixelsY, int width, int height);

	private:
		std::filesystem::path mCurrentPath;
		std::vector<std::filesystem::path> mEntries;

		size_t mSelectedIndex;
		bool mChangingCurrentPath;
		bool mProgramSelected;

		TTF_Font* mFont;
	};
}

#endif // CHIP8_SYSTEM_H