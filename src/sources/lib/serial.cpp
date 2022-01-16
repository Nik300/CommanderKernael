#include <lib/serial.h>
#include <stdarg.h>
#include <std/stdio.h>
#include <std/cstring.h>

namespace System::IO::Serial
{
	SerialPort port = SerialPort::COM1;
	bool enabled = false;

	int is_transmit_empty() {
   		return inb((uint16_t)port + 5) & 0x20;
	}

	void Init(SerialPort port)
	{
		Serial::port = port;
		enabled = true;

		outb((uint16_t)port + 1, 0x00);
		outb((uint16_t)port + 3, 0x80);
		outb((uint16_t)port + 0, 0x03);
		outb((uint16_t)port + 1, 0x00);
		outb((uint16_t)port + 3, 0x03);
		outb((uint16_t)port + 2, 0xC7);
		outb((uint16_t)port + 4, 0x0B);
	}
	void Disable()
	{
		Serial::enabled = false;

		outb((uint16_t)port + 1, 0x00);
	}

	void __print(char *text)
	{
		if (!Serial::enabled)
			return;
		for (int i = 0; text[i] != '\0'; i++)
		{
			while (!is_transmit_empty());
			outb((uint16_t)port, text[i]);
		}
	}

	void __print_v(const char *text, va_list args)
	{
		if (!Serial::enabled)
			return;
		for (int i = 0; i < strlen(text); i++)
		{
			if (text[i] == '%')
			{
				i++;
				if (text[i] == 'd')
				{
					char buf[1000];
					numstr_buf(va_arg(args, int), 10, buf);
					__print(buf);
				}
				else if (text[i] == 's')
				{
					char* value = va_arg(args, char*);
					__print(value);
				}
				else if (text[i] == 'c')
				{
					char value = va_arg(args, int);
					char buf[2] = {value, '\0'};
					__print(buf);
				}
				else if (text[i] == 'h')
				{
					char value[10000];
					numstr_buf(va_arg(args, int), 16, value);
					__print(value);
				}
				else if (text[i] == 'x')
				{
					char value[10000];
					numstr_buf(va_arg(args, int), 16, value);
					__print(value);
				}
				else if (text[i] == 'b')
				{
					char value[10000];
					numstr_buf(va_arg(args, int), 10, value);
					__print(value);
				}
				else if (text[i] == 'o')
				{
					char value[10000];
					numstr_buf(va_arg(args, int), 8, value);
					__print(value);
				}
				else if (text[i] == '%')
				{
					__print("%");
				}
			}
			else
			{
				char buf[2] = {text[i], '\0'};
				__print(buf);
			}
		}
	}

	void Write(const char *str, ...)
	{
		if (!Serial::enabled)
			return;
		va_list args;
		va_start(args, str);
		__print_v(str, args);
		va_end(args);
	}
	void WriteLine(const char *str, ...)
	{
		if (!Serial::enabled)
			return;
		va_list args;
		va_start(args, str);
		__print_v(str, args);
		va_end(args);
		__print("\n");
	}
}

__cdecl void dprint(const char *str)
{
	if (!System::IO::Serial::enabled)
		return;
	using namespace System::IO;
	Serial::Write(str);
}
__cdecl void dprintf(const char *fmt, ...)
{
	if (!System::IO::Serial::enabled)
		return;
	va_list args;
	va_start(args, fmt);
	using namespace System::IO;
	Serial::__print_v(fmt, args);
	va_end(args);
}