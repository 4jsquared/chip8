#ifndef CHIP8_PROGRAM_H
#define CHIP8_PROGRAM_H

#include "display.h"
#include "image.h"
#include "process.h"
#include "timer.h"

#include <cstddef>
#include <vector>

namespace chip8
{
	class Program : public Process
	{
		// See https://en.wikipedia.org/wiki/CHIP-8
	public:
		Program(Image&& image);

		void Render(SDL_Renderer* renderer) override;
		bool Finished() override { return false; };

		void OnKeyDown(const SDL_Keysym& keysym) override;
		void OnKeyUp(const SDL_Keysym& keysym) override;

	private:
		void Execute(uint32_t opcodeCount);

		void ExecuteOpcode(uint16_t opcode);

		// Sub categories of opcodes
		void ExecuteOpcode0(uint16_t opcode);
		void ExecuteOpcode8(uint16_t opcode);
		void ExecuteOpcodeD(uint16_t opcode);
		void ExecuteOpcodeE(uint16_t opcode);
		void ExecuteOpcodeF(uint16_t opcode);

	private:
		// System
		Display mDisplay;
		std::chrono::system_clock::time_point mLastExecution;

		// Memory
		Image mImage;

		// Registers
		static constexpr size_t  kNumRegisters  = 16;
		static constexpr uint8_t kCarryRegister = 0xF;
		uint8_t  mRegister[kNumRegisters] = {};
		uint16_t mAddressRegister         = 0;
		uint16_t mProgramCounter          = 0;

		// Stack
		std::vector<uint16_t> mStack;

		// Timers
		Timer mDelayTimer;
		Timer mSoundTimer; // TODO - this needs to set off a bell when it hits 0
	};
}

#endif // CHIP8_PROGRAM_H