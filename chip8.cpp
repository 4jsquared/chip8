#include "system.h"

#include "SDL_main.h"

int main(int argc, char* argv[])
{
	chip8::System system;
	system.Run();

	return 0;
}
