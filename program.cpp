#include "program.h"

#include "log.h"

#include <cassert>
#include <cstdint>
#include <cstring>

#include <iterator>

namespace
{
	// Helpers for extracting information from opcodes
	uint8_t OpRegisterX(uint16_t opcode)
	{
		return (opcode & 0x0F00) >> 8;
	}

	uint8_t OpRegisterY(uint16_t opcode)
	{
		return (opcode & 0x00F0) >> 4;
	}

	uint16_t OpAddress(uint16_t opcode)
	{
		return opcode & 0x0FFF;
	};

	uint8_t OpValue(uint16_t opcode)
	{
		return opcode & 0x00FF;
	};
}

namespace chip8
{
	Program::Program(Image&& image)
		: mImage(std::move(image))
	{
		mProgramCounter = mImage.StartOffset();

		// Start execution on another thread
		mThread = std::thread([this]() {
			Execute();
		});
	}

	void Program::Render(SDL_Renderer* renderer)
	{
		std::unique_lock<std::mutex> lock(mDisplayMutex);
		mDisplay.Render(renderer);
	}

	void Program::OnKeyDown(const SDL_Keysym& keysym)
	{

	}

	void Program::OnKeyUp(const SDL_Keysym& keysym)
	{

	}

	void Program::Execute()
	{
		while (true)
		{
			uint16_t opcode = (static_cast<uint16_t>(mImage[mProgramCounter]) << 8) + mImage[mProgramCounter + 1];

			// Move the program counter so that it points to the next operation.
			// Individual opcodes such as jump or call may change this.
			mProgramCounter += 2;

			ExecuteOpcode(opcode);
		}
	}

	void Program::ExecuteOpcode(uint16_t opcode)
	{
		switch ((opcode & 0xF000) >> 12)
		{
		case 0x0: // Functions
			ExecuteOpcode0(opcode);
			break;
		case 0x1: // 1NNN: Jump to NNN
			mProgramCounter = OpAddress(opcode);
			break;
		case 0x2: // 2NNN: Call NNN
			// Only the program counter is saved, the registers are ignored
			mStack.push_back(mProgramCounter);
			mProgramCounter = opcode & 0x0FFF;
			break;
		case 0x3: // 3XNN: Skip if register X equal NN
			if (mRegister[OpRegisterX(opcode)] == OpValue(opcode))
				mProgramCounter += 2;
			break;
		case 0x4: // 4XNN: Skip if register X not equal NN
			if (mRegister[OpRegisterX(opcode)] != OpValue(opcode))
				mProgramCounter += 2;
			break;
		case 0x5: // 5XYN: Skip if register X equals register Y
			assert((opcode & 0x000F) == 0); // What would this mean?
			if (mRegister[OpRegisterX(opcode)] == mRegister[OpRegisterY(opcode)])
				mProgramCounter += 2;
			break;
		case 0x6: // 6XNN: register X = NN
			mRegister[OpRegisterX(opcode)] = OpValue(opcode);
			break;
		case 0x7: // 7XNN: register X += NN, no carry
			mRegister[OpRegisterX(opcode)] += OpValue(opcode);
			break;
		case 0x8: // 8XYN: Two register operations
			ExecuteOpcode8(opcode);
			break;
		case 0xA: // ANNN: Mem register = NNN
			mAddressRegister = OpAddress(opcode);
			break;
		case 0xB: // BNNN: Jump to v0 + NNN
			mProgramCounter = mRegister[0] + OpAddress(opcode);
			break;
		case 0xC: // CXNN: Register X = rand & NN
			mRegister[OpRegisterX(opcode)] = rand() & OpValue(opcode);
			break;
		case 0xD:
			ExecuteOpcodeD(opcode);
			break;
		case 0xE:
			ExecuteOpcodeE(opcode);
			break;
		case 0xF:
			ExecuteOpcodeF(opcode);
			break;
		}
	}

	void Program::ExecuteOpcode0(uint16_t opcode)
	{
		assert((opcode & 0xF000) == 0x0000);

		switch (opcode)
		{
		case 0x00E0: // Clear the display
		{
			std::unique_lock<std::mutex> lock(mDisplayMutex);
			mDisplay.Clear();
		}
			break;
		case 0x00EE: // Return
			assert(!mStack.empty());
			mProgramCounter = mStack.back();
			mStack.pop_back();
			break;
		default: // Old machine codes, not needed for interpreting the program?
			LOG("Ignoring opcode %u", opcode);
			break;
		}
	}

	void Program::ExecuteOpcode8(uint16_t opcode)
	{
		assert((opcode & 0xF000) == 0x8000);

		uint8_t& regX = mRegister[OpRegisterX(opcode)];
		uint8_t& regY = mRegister[OpRegisterY(opcode)];

		bool xOrYIsCarry = OpRegisterX(opcode) == kCarryRegister || OpRegisterY(opcode) == kCarryRegister;

		switch (opcode & 0xF)
		{
		case 0x0: // 8XY0: register X = register Y
			regX = regY;
			break;
		case 0x1: // 8XY1: register X |= register Y
			regX |= regY;
			break;
		case 0x2: // 8XY2: register X &= register Y
			regX &= regY;
			break;
		case 0x3: // 8XY3: register X ^= register Y
			regX ^= regY;
			break;
		case 0x4: // 8XY4: register X += register Y, with carry
		{
			assert(!xOrYIsCarry); // What happens if the carry register is also one of the register being used?
			uint16_t result = static_cast<uint16_t>(regX) + static_cast<uint16_t>(regY);
			regX = static_cast<uint8_t>(result & 0xFF);
			mRegister[kCarryRegister] = result > 0xFF ? 1 : 0;
		}
		break;
		case 0x5: // 8XY5: register X -= register Y, carry = !borrow
		{
			assert(!xOrYIsCarry); // What happens if the carry register is also one of the register being used?
			uint16_t result = static_cast<uint16_t>(regX) - static_cast<uint16_t>(regY);
			regX = static_cast<uint8_t>(result & 0xFF);
			mRegister[kCarryRegister] = result > 0xFF ? 0 : 1;
		}
		break;
		case 0x6: // 8XY6: register X >>= 1, carry = register X & 1;
			assert(OpRegisterX(opcode) == OpRegisterY(opcode));
			assert(!xOrYIsCarry);
			mRegister[kCarryRegister] = regX & 0x1;
			regX >>= 1;
			break;
		case 0x7: // 8XY7: register X = register Y - register X, carry = !borrow
		{
			assert(!xOrYIsCarry);
			uint16_t result = static_cast<uint16_t>(regX) - static_cast<uint16_t>(regY);
			regX = static_cast<uint8_t>(result & 0xFF);
			mRegister[kCarryRegister] = result > 0xFF ? 0 : 1;
		}
		break;
		case 0xE: // 8XYE: register X <<= 1, carry = register X highest bit;
			assert((opcode & 0x000) == 0);
			assert(!xOrYIsCarry);
			mRegister[kCarryRegister] = regX >> 7;
			regX <<= 1;
			break;
		default:
			assert(false); // TODO
			break;
		}
	}

	void Program::ExecuteOpcodeD(uint16_t opcode)
	{
		// DXYN - Display sprite (from memomry address register) at coordinates given by registers X and Y
		// The sprite is 8 pixels wide, and N pixels high
		assert((opcode & 0xF000) == 0xD000);

		constexpr uint8_t width = 8;
		uint8_t height = opcode & 0x000F;
		uint8_t x = mRegister[OpRegisterX(opcode)];
		uint8_t y = mRegister[OpRegisterY(opcode)];

		assert(mAddressRegister + height * width < sizeof(mImage));

		bool flipped = false;
		{
			std::unique_lock<std::mutex> lock(mDisplayMutex);
			flipped = mDisplay.Draw(x, y, height, &mImage[mAddressRegister]);
		}

		// The carry bit is set depending on whether any pixels were turned off
		mRegister[kCarryRegister] = flipped ? 1 : 0;
	}

	void Program::ExecuteOpcodeE(uint16_t opcode)
	{
		// EXNN - Functions using one register
		assert((opcode & 0xF000) == 0xE000);

		uint8_t registerIndex = OpRegisterX(opcode);

		switch (opcode & 0xFF)
		{
		case 0x9E: // EX9E - Skip if key in VX is pressed
			// Skip for now, assuming no input
			break;
		case 0xA1: // EX9E - Skip if key in VX isn't pressed
			// Skip for now, assuming no input
			mProgramCounter += 2;
			break;
		default:
			assert(false); // TODO
			break;
		}
	}

	void Program::ExecuteOpcodeF(uint16_t opcode)
	{
		// FXNN - Functions using one register
		assert((opcode & 0xF000) == 0xF000);

		uint8_t registerIndex = OpRegisterX(opcode);

		switch (opcode & 0xFF)
		{
		case 0x07: // FX07 - Get the delay timer to register X
			mRegister[registerIndex] = mDelayTimer.GetValue();
			break;
		case 0x15: // FX15 - Set the delay timer to register X
			mDelayTimer.SetValue(mRegister[registerIndex]);
			break;
		case 0x18: // FX18 - Set the sound timer to register X
			mSoundTimer.SetValue(mRegister[registerIndex]);
			break;
		case 0x29: // FX29 - Set address register to sprite for character in register X
		{
			uint8_t value = mRegister[registerIndex];
			mAddressRegister = mImage.SpriteOffset(value);
		}
			break;
		case 0x33: // FX33 - Dump BCD encoding to memory, most signifcant first
			mImage[mAddressRegister] = (mRegister[registerIndex] / 100);
			mImage[mAddressRegister + 1] = (mRegister[registerIndex] / 10) % 10;
			mImage[mAddressRegister + 2] = mRegister[registerIndex] % 10;
			break;
		case 0x55: // FX55 - Dump registers 0 to X (inclusive) to memory
			std::copy_n(mRegister, registerIndex + 1, &mImage[mAddressRegister]);
			break;
		case 0x65: // FX65 - Load registers 0 to X (inclusive) to memory
			std::copy_n(&mImage[mAddressRegister], registerIndex + 1, mRegister);
			break;
		default:
			assert(false); // TODO
			break;
		}
	}
}
