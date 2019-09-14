#include "program_select.h"

#include "image.h"
#include "program.h"

#include <cassert>

namespace
{
	constexpr char kImageExtension[] = ".ch8";
	constexpr char kFont[] = "DejaVuSans.ttf";
	constexpr int kFontSize = 12;

	const SDL_Color kTextColor{ 0xFF, 0xFF, 0xFF, 0xFF };
	const SDL_Color kHighlightedTextColor{ 0x00, 0x00, 0xFF, 0xFF };
}

namespace chip8
{
	ProgramSelect::ProgramSelect()
		: mCurrentPath(std::filesystem::current_path())
		, mSelectedIndex(0)
		, mChangingCurrentPath(true)
		, mProgramSelected(false)
	{
		mFont = TTF_OpenFont(kFont, kFontSize);
		assert(mFont != nullptr);
	}

	ProgramSelect::~ProgramSelect()
	{
		TTF_CloseFont(mFont);
	}

	void ProgramSelect::Render(SDL_Renderer* renderer)
	{
		// Make sure that the paths are up to date
		UpdatePaths();

		// Clear anything on the display
		int result = SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
		assert(result == 0);
		result = SDL_RenderClear(renderer);
		assert(result == 0);

		// Check how much space we have
		int width, height;
		result = SDL_GetRendererOutputSize(renderer, &width, &height);
		assert(result == 0);

		// Get the selected entry
		assert(mSelectedIndex < mEntries.size());
		SDL_Surface* surface = CreateTextSurface(mEntries[mSelectedIndex], kHighlightedTextColor);

		int pixelsStart = (height / 2) - (surface->h / 2);
		int pixelsEnd = pixelsStart + surface->h;

		RenderSurface(renderer, surface, pixelsStart, width, height);
		SDL_FreeSurface(surface);

		// Then add entries above
		for (int i = mSelectedIndex - 1; i >= 0 && pixelsStart > 0; i--)
		{
			SDL_Surface* surface = CreateTextSurface(mEntries[i], kTextColor);
			pixelsStart -= surface->h;
			RenderSurface(renderer, surface, pixelsStart, width, height);
			SDL_FreeSurface(surface);
		}

		// Then add entries below
		for (int i = mSelectedIndex + 1; i < mEntries.size() && pixelsEnd < height; i++)
		{
			SDL_Surface* surface = CreateTextSurface(mEntries[i], kTextColor);
			RenderSurface(renderer, surface, pixelsEnd, width, height);
			pixelsEnd += surface->h;
			SDL_FreeSurface(surface);
		}
	}

	bool ProgramSelect::Finished()
	{
		return mProgramSelected;
	}

	std::unique_ptr<Process> ProgramSelect::NextProcess()
	{
		Image image(mCurrentPath);
		return std::make_unique<Program>(std::move(image));
	}

	void ProgramSelect::OnKeyUp(const SDL_Keysym& keysym)
	{
		if (!mChangingCurrentPath)
		{
			// If we aren't in the process of changing the current path,
			// navigate the available selections
			if (keysym.scancode == SDL_SCANCODE_UP)
			{
				if (mSelectedIndex > 0)
					mSelectedIndex--;
				else
					mSelectedIndex = mEntries.size() - 1;
			}
			else if (keysym.scancode == SDL_SCANCODE_DOWN)
			{
				if (mSelectedIndex < mEntries.size() - 1)
					mSelectedIndex++;
				else
					mSelectedIndex = 0;
			}
			else if (keysym.scancode == SDL_SCANCODE_RETURN)
			{
				mChangingCurrentPath = true;
			}
		}
	}

	void ProgramSelect::UpdatePaths()
	{
		if (mChangingCurrentPath)
		{
			if (mSelectedIndex < mEntries.size())
				mCurrentPath = mEntries[mSelectedIndex];

			std::filesystem::directory_entry currentEntry(mCurrentPath);
			if (currentEntry.is_regular_file())
			{
				// This is a file - attempt to load it as a program
				mProgramSelected = true;
			}
			else
			{
				assert(currentEntry.is_directory());

				mEntries.clear();
				// Add the parent path - we want this at the top of the list to navigate up one
				mEntries.push_back(mCurrentPath.parent_path());

				// Then add valid children
				for (const auto& child : std::filesystem::directory_iterator(mCurrentPath))
				{
					if (child.is_directory() ||
						(child.is_regular_file() && child.path().extension() == kImageExtension))
					{
						mEntries.push_back(child.path());
					}
				}

				mChangingCurrentPath = false;
				mSelectedIndex = 0;
			}
		}
	}

	SDL_Surface* ProgramSelect::CreateTextSurface(const std::filesystem::path& path, const SDL_Color& color)
	{
#if _WIN32
		SDL_Surface* surface = TTF_RenderUNICODE_Solid(mFont, reinterpret_cast<const Uint16*>(path.filename().c_str()), color);
#else
		SDL_Surface* surface = TTF_RenderUTF8_Solid(mFont, path.filename().c_str(), color);
#endif
		assert(surface != nullptr);
		return surface;
	}

	void ProgramSelect::RenderSurface(SDL_Renderer* renderer, SDL_Surface* surface, int pixelsY, int width, int height)
	{
		// TODO: This is almost certainly horrifically inefficient - should at least cache textures.
		assert(surface != nullptr);

		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
		assert(texture != nullptr);

		int copyStart = std::max(0, pixelsY);
		int copyWidth = std::min(surface->w, width);
		int copyHeight = std::min({ surface->h, height - pixelsY, pixelsY + surface->h }); // Surface height, pixels to end, pixels from start
		SDL_Rect srcRect{ 0, 0, copyWidth, copyHeight };
		SDL_Rect dstRect{ 0, copyStart, copyWidth, copyHeight };

		int result = SDL_RenderCopy(renderer, texture, &srcRect, &dstRect);
		assert(result == 0);
		SDL_DestroyTexture(texture);
	};
}
