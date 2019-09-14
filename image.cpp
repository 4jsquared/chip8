#include "image.h"

#include <algorithm>
#include <cassert>
#include <cstdio>

namespace
{
	// Built-in sprite - see http://devernay.free.fr/hacks/chip8/C8TECH10.HTM#8xy6
	struct BuiltinSprite
	{
		uint8_t data[5];
	};

	constexpr BuiltinSprite sBuiltinSprites[] = {
		{
			0b11110000,
			0b10010000,
			0b10010000,
			0b10010000,
			0b11110000,
		},
		{
			0b00100000,
			0b01100000,
			0b00100000,
			0b00100000,
			0b01110000,
		},
		{
			0b11110000,
			0b00010000,
			0b11110000,
			0b10000000,
			0b11110000,
		},
		{
			0b11110000,
			0b00010000,
			0b11110000,
			0b00010000,
			0b11110000,
		},
		{
			0b10010000,
			0b10010000,
			0b11110000,
			0b00010000,
			0b00010000,
		},
		{
			0b11110000,
			0b10000000,
			0b10010000,
			0b00010000,
			0b11110000,
		},
		{
			0b11110000,
			0b10000000,
			0b11110000,
			0b10010000,
			0b11110000,
		},
		{
			0b11110000,
			0b00010000,
			0b00100000,
			0b01000000,
			0b01000000,
		},
		{
			0b11110000,
			0b10010000,
			0b11110000,
			0b10010000,
			0b11110000,
		},
		{
			0b11110000,
			0b10010000,
			0b11110000,
			0b00010000,
			0b11110000,
		},
		{
			0b11110000,
			0b10010000,
			0b11110000,
			0b10010000,
			0b10010000,
		},
		{
			0b11100000,
			0b10010000,
			0b11100000,
			0b10010000,
			0b11100000,
		},
		{
			0b11110000,
			0b10000000,
			0b10000000,
			0b10000000,
			0b11110000,
		},
		{
			0b11100000,
			0b10010000,
			0b10010000,
			0b10010000,
			0b11100000,
		},
		{
			0b11110000,
			0b10000000,
			0b11110000,
			0b10000000,
			0b11110000,
		},
		{
			0b11110000,
			0b10000000,
			0b11110000,
			0b10000000,
			0b10000000,
		},
	};

	constexpr size_t kSpriteStart  = 0x000;
	constexpr size_t kProgramStart = 0x200;

	static_assert(kSpriteStart + sizeof(sBuiltinSprites) <= kProgramStart);
}

namespace chip8
{
	Image::Image(const std::filesystem::path& path)
		: mData{}
	{
#ifdef _WIN32
		FILE* file = _wfopen(path.c_str(), L"rb");
#else
		FILE* file = fopen(filePath.c_str(), "rb");
#endif
		assert(file != nullptr);

		int result = fseek(file, 0, SEEK_END);
		assert(result == 0);

		long fileSize = ftell(file);
		assert(fileSize > 0);
		assert(fileSize <= kImageSize - kProgramStart);
		result = fseek(file, 0, SEEK_SET);
		assert(fileSize);

		// Read the file data into place in the image
		size_t readElements = fread(mData + kProgramStart, fileSize, 1, file);
		assert(readElements == 1);

		fclose(file);

		// Copy sprites into the expected location
		memcpy(mData + kSpriteStart, sBuiltinSprites, sizeof(sBuiltinSprites));
	}

	uint16_t Image::StartOffset()
	{
		return kProgramStart;
	}

	uint16_t Image::SpriteOffset(uint8_t index)
	{
		assert(index < std::size(sBuiltinSprites));
		return kSpriteStart + sizeof(BuiltinSprite) * index;
	}
}