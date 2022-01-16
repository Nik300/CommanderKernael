#include <drivers/keyboard.h>

#include <std/logging.h>

#include <lib/memory.h>
#include <lib/ports.h>

using namespace System::HAL;

char* *KeyboardDriver::layout = nullptr;

KeyboardDriver::KeyboardDriver()
: InterruptDriver(0x21, handler), handler([](regs32_t *regs)
{
	uint8_t scancode = inb(0x60);
	
	if (scancode >= 58)
		return;
	
	char c[2] = {layout[scancode][0], 0};

	print(c);
})
{
	// set the italian layout
	layout = new char*[58]
	{
		"\0", "\0", "1", "2", "3", "4", "5", "6",
		"7", "8", "9", "0", "'", "ì", "\b", "\t", "q", "w", "e", "r", "t", "y",
		"u", "i", "o", "p", "è", "+", "\n", "", "a", "s", "d", "f", "g",
		"h", "j", "k", "l", "ò", "à", "ù", "", "<", "z", "x", "c", "v",
        "b", "n", "m", ",", ".", "-", "\0", "\0", "\0", " "
	};
}
KeyboardDriver::~KeyboardDriver()
{
	delete layout;
}

void KeyboardDriver::Init()
{
}
void KeyboardDriver::Activate()
{
	register_int_handler(1, handler);
}
void KeyboardDriver::Deactivate()
{
	register_int_handler(1, nullptr);
}
void KeyboardDriver::Reset()
{
	Deactivate();
	Init();
	Activate();
}