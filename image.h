#ifndef CHIP8_IMAGE_H
#define CHIP8_IMAGE_H

#include <cassert>
#include <cstdlib>
#include <filesystem>

namespace chip8
{
	class Image
	{
		// 4K memory available
		static constexpr size_t kImageSize = 4 * 1024;

	public:
		Image(const std::filesystem::path& path);

		uint16_t StartOffset();
		uint16_t SpriteOffset(uint8_t index);

		uint8_t& operator[](size_t offset) {
			assert(offset < kImageSize);
			return mData[offset];
		}

	private:
		uint8_t mData[kImageSize];
	};
}

#endif // CHIP8_SYSTEM_H