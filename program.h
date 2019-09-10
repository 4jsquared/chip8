#ifndef CHIP8_PROGRAM_H
#define CHIP8_PROGRAM_H

#include "display.h"
#include "timer.h"

#include <cstddef>
#include <vector>

namespace chip8
{
	class Program
	{
		// See https://en.wikipedia.org/wiki/CHIP-8
	public:
		Program(const void* data, size_t numBytes);

		void Execute();

	private:
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

		// Memory
		static constexpr size_t kMemoryNumBytes = 4 * 1024;
		static constexpr size_t kProgramStart = 0x200;
		uint8_t mMemory[kMemoryNumBytes] = {};

		// Registers
		static constexpr size_t kNumRegisters = 16;
		static constexpr uint8_t kCarryRegister = 0xF;
		uint8_t mRegister[kNumRegisters] = {};
		uint16_t mAddressRegister = 0;
		uint16_t mProgramCounter = kProgramStart;

		// Stack
		std::vector<uint16_t> mStack;

		// Timers
		Timer mDelayTimer;
		Timer mSoundTimer; // TODO - this needs to set off a bell when it hits 0
	};
}

#endif // CHIP8_PROGRAM_H