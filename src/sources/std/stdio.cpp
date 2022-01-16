#include <std/stdio.h>
#include <std/cstring.h>
#include <std/stddecl.h>
#include <stddef.h>

#include <lib/ports.h>
#include <lib/memory.h>
#include <lib/serial.h>
#define GET_OFFSET(x,y, MAX_COLS) (2 * ((y) * (MAX_COLS) + (x)))
void *stdio = (void*)0xb8000;
System::IO::ConsoleColor fg = System::IO::ConsoleColor::White;
System::IO::ConsoleColor bg = System::IO::ConsoleColor::Black;
__cdecl int print(const char* text)
{
	using System::IO::Console;
	return Console::Write(text);
}
__cdecl int printf(const char* format, ...)
{
	using System::IO::Console;
	va_list args;
	va_start(args, format);
	int ret = Console::WriteV(format, args);
	va_end(args);
	return ret;
}
__cdecl int printv(const char* format, va_list args)
{
	using System::IO::Console;
	return Console::WriteV(format, args);
}

namespace System::IO
{
	#pragma region CONSOLE
	ConsoleStream::ConsoleStream()
	: columns(0), rows(0), buffer((char*)stdio), cursor()
	{
	}
	ConsoleStream::~ConsoleStream()
	{
		columns = 0;
		rows = 0;
		buffer = nullptr;
	}
	int ConsoleStream::read(char* buffer, int length)
	{
		return -1;
	}
	int ConsoleStream::write(const char* buffer, int length)
	{
		for (int i = 0; i < length; i++)
		{
			if (buffer[i] == '\n')
			{
				cursor.move(0, cursor.get_y() + 1);
				if (GET_OFFSET(cursor.get_x(), cursor.get_y(), columns) >= 2*(rows * columns))
				{
					for (int r = 0; r < rows; r++)
					{
						memcpyw(this->buffer + (GET_OFFSET(0, r, columns)), this->buffer + (GET_OFFSET(0, r+1, columns)), 4*columns);
					}
					memsetw(this->buffer + (GET_OFFSET(0, rows-1, columns)), 0x0F00, 2*columns);
					cursor.move(0, rows-1);
				}
			}
			else if (buffer[i] == '\b')
			{
				if (cursor.get_x() > 0)
				{
					cursor.move(cursor.get_x() - 1, cursor.get_y());
					memsetw(this->buffer + (GET_OFFSET(cursor.get_x(), cursor.get_y(), columns)), 0x0F00, 2);
				}
				else
				{
					if (cursor.get_y() == 0 && cursor.get_x() == 0) continue;
					int last_x;
					int y = cursor.get_y()-1;
					for (int c = 0; c < columns; c++)
					{
						if (this->buffer[GET_OFFSET(c, y, columns)] == 0x00)
						{
							last_x = c;
							break;
						}
					}
					cursor.move(last_x, y);
					memsetw(this->buffer + (GET_OFFSET(cursor.get_x(), cursor.get_y(), columns)), 0x0F00, 2);
				}
			}
			else
			{
				uint8_t color = ((uint8_t)bg << 4) | (uint8_t)fg;
				this->buffer[GET_OFFSET(cursor.get_x(), cursor.get_y(), columns)+1] = color;
				this->buffer[GET_OFFSET(cursor.get_x(), cursor.get_y(), columns)] = buffer[i];
				cursor.moveX(cursor.get_x() + 1);
				if (GET_OFFSET(cursor.get_x(), cursor.get_y(), columns) >= 2*(rows * columns))
				{
					for (int r = 0; r < rows; r++)
					{
						memcpyw(this->buffer + (GET_OFFSET(0, r, columns)), this->buffer + (GET_OFFSET(0, r+1, columns)), 4*columns);
					}
					memsetw(this->buffer + (GET_OFFSET(0, rows-1, columns)), 0x0F00, 2*columns);
					cursor.move(0, rows-1);
				}
			}
		}
		return length;
	}
	ConsoleStream Console::console = ConsoleStream();
	int Console::WriteV(const char* text, va_list args)
	{
		int chars_written = 0;
		for (int i = 0; i < strlen(text); i++)
		{
			if (text[i] == '%')
			{
				i++;
				if (text[i] == 'd')
				{
					char *buf = decstr(va_arg(args, int));
					console.write(buf, strlen(buf));
					chars_written += strlen(buf);
					free(buf);
				}
				else if (text[i] == 's')
				{
					char* value = va_arg(args, char*);
					console.write(value, strlen(value));
					chars_written += strlen(value);
				}
				else if (text[i] == 'c')
				{
					char value = va_arg(args, int);
					console.write(&value, 1);
					chars_written += 1;
				}
				else if (text[i] == 'h')
				{
					char* value = hexstr(va_arg(args, int));
					console.write(value, strlen(value));
					chars_written += strlen(value);
					free(value);
				}
				else if (text[i] == 'x')
				{
					char* value = hexstr(va_arg(args, int));
					console.write(value, strlen(value));
					chars_written += strlen(value);
					free(value);
				}
				else if (text[i] == 'b')
				{
					char* value = binstr(va_arg(args, int));
					console.write(value, strlen(value));
					chars_written += strlen(value);
					free(value);
				}
				else if (text[i] == 'o')
				{
					char* value = octstr(va_arg(args, double));
					console.write(value, strlen(value));
					chars_written += strlen(value);
					free(value);
				}
				else if (text[i] == '%')
				{
					console.write("%", 1);
					chars_written++;
				}
			}
			else
			{
				console.write(&text[i], 1);
				chars_written++;
			}
		}
		va_end(args);
		return chars_written;
	}
	int Console::Write(const char* text, ...)
	{
		va_list args;
		va_start(args, text);
		int chars_written = 0;
		for (int i = 0; i < strlen(text); i++)
		{
			if (text[i] == '%')
			{
				i++;
				if (text[i] == 'd')
				{
					char *buf = decstr(va_arg(args, int));
					console.write(buf, strlen(buf));
					chars_written += strlen(buf);
					free(buf);
				}
				else if (text[i] == 's')
				{
					char* value = va_arg(args, char*);
					console.write(value, strlen(value));
					chars_written += strlen(value);
				}
				else if (text[i] == 'c')
				{
					char value = va_arg(args, int);
					console.write(&value, 1);
					chars_written += 1;
				}
				else if (text[i] == 'h')
				{
					char* value = hexstr(va_arg(args, int));
					console.write(value, strlen(value));
					chars_written += strlen(value);
					free(value);
				}
				else if (text[i] == 'x')
				{
					char* value = hexstr(va_arg(args, int));
					console.write(value, strlen(value));
					chars_written += strlen(value);
					free(value);
				}
				else if (text[i] == 'b')
				{
					char* value = binstr(va_arg(args, int));
					console.write(value, strlen(value));
					chars_written += strlen(value);
					free(value);
				}
				else if (text[i] == 'o')
				{
					char* value = octstr(va_arg(args, double));
					console.write(value, strlen(value));
					chars_written += strlen(value);
					free(value);
				}
				else if (text[i] == '%')
				{
					console.write("%", 1);
					chars_written++;
				}
			}
			else
			{
				console.write(&text[i], 1);
				chars_written++;
			}
		}
		va_end(args);
		return chars_written;
	}
	int Console::Write(ConsoleColor Color,const char* text, ...)
	{
		
		ConsoleColor CurrentColor = GetFG();
		SetFG(Color);
		va_list args;
		va_start(args, text);
		int chars_written = WriteV(text,args);
		va_end(args);
		SetFG(CurrentColor);
		return chars_written;
	}
	int Console::WriteLine(const char* text, ...)
	{
		va_list args;
		va_start(args, text);
		int chars_written = 0;
		for (int i = 0; i < strlen(text); i++)
		{
			if (text[i] == '%')
			{
				i++;
				if (text[i] == 'd')
				{
					char *buf = decstr(va_arg(args, int));
					console.write(buf, strlen(buf));
					chars_written += strlen(buf);
					free(buf);
				}
				else if (text[i] == 's')
				{
					char* value = va_arg(args, char*);
					console.write(value, strlen(value));
					chars_written += strlen(value);
				}
				else if (text[i] == 'c')
				{
					char value = va_arg(args, int);
					console.write(&value, 1);
					chars_written += 1;
				}
				else if (text[i] == 'h')
				{
					char* value = hexstr(va_arg(args, int));
					console.write(value, strlen(value));
					chars_written += strlen(value);
					free(value);
				}
				else if (text[i] == 'x')
				{
					char* value = hexstr(va_arg(args, int));
					console.write(value, strlen(value));
					chars_written += strlen(value);
					free(value);
				}
				else if (text[i] == 'b')
				{
					char* value = binstr(va_arg(args, int));
					console.write(value, strlen(value));
					chars_written += strlen(value);
					free(value);
				}
				else if (text[i] == 'o')
				{
					char* value = octstr(va_arg(args, double));
					console.write(value, strlen(value));
					chars_written += strlen(value);
					free(value);
				}
				else if (text[i] == '%')
				{
					console.write("%", 1);
					chars_written++;
				}
			}
			else
			{
				console.write(&text[i], 1);
				chars_written++;
			}
		}
		va_end(args);
		console.write("\n", 1);
		chars_written++;
		return chars_written;
	}
	int Console::WriteLine(ConsoleColor color,const char* text, ...)
	{
		ConsoleColor CurrentColor = GetFG();
		SetFG(color);
		va_list args;
		va_start(args, text);
		int chars_written = 0;
		for (int i = 0; i < strlen(text); i++)
		{
			if (text[i] == '%')
			{
				i++;
				if (text[i] == 'd')
				{
					char *buf = decstr(va_arg(args, int));
					console.write(buf, strlen(buf));
					chars_written += strlen(buf);
					free(buf);
				}
				else if (text[i] == 's')
				{
					char* value = va_arg(args, char*);
					console.write(value, strlen(value));
					chars_written += strlen(value);
				}
				else if (text[i] == 'c')
				{
					char value = va_arg(args, int);
					console.write(&value, 1);
					chars_written += 1;
				}
				else if (text[i] == 'h')
				{
					char* value = hexstr(va_arg(args, int));
					console.write(value, strlen(value));
					chars_written += strlen(value);
					free(value);
				}
				else if (text[i] == 'x')
				{
					char* value = hexstr(va_arg(args, int));
					console.write(value, strlen(value));
					chars_written += strlen(value);
					free(value);
				}
				else if (text[i] == 'b')
				{
					char* value = binstr(va_arg(args, int));
					console.write(value, strlen(value));
					chars_written += strlen(value);
					free(value);
				}
				else if (text[i] == 'o')
				{
					char* value = octstr(va_arg(args, double));
					console.write(value, strlen(value));
					chars_written += strlen(value);
					free(value);
				}
				else if (text[i] == '%')
				{
					console.write("%", 1);
					chars_written++;
				}
			}
			else
			{
				console.write(&text[i], 1);
				chars_written++;
			}
		}
		va_end(args);
		console.write("\n", 1);
		chars_written++;
		SetFG(CurrentColor);
		return chars_written;
	}
	const char* Console::ReadLine()
	{
		return nullptr;
	}
	void Console::Clear()
	{
		console.cursor.move(0, 0);
		uint8_t color = ((uint8_t)bg << 4) | (uint8_t)fg;
		memsetw(console.buffer, color << 8, 2*(console.columns*console.rows));
	}
	void Console::Clear(ConsoleColor fg, ConsoleColor bg)
	{
		console.cursor.move(0, 0);
		uint8_t color = ((uint8_t)bg << 4) | (uint8_t)fg;
		memsetw(console.buffer, color << 8, 2*(console.columns*console.rows));
	}
	void Console::Init(void *io)
	{
		fg = ConsoleColor::White;
		bg = ConsoleColor::Black;
		console = ConsoleStream();
		console.buffer = reinterpret_cast<char*>(io);
		console.columns = 80;
		console.rows = 25;
	}
	#pragma endregion
	#pragma region CURSOR
	TextModeCursor::TextModeCursor()
	: x(inb(0x3D4) & 0xFF), y(inb(0x3D5) & 0xFF)
	{
	}
	TextModeCursor::~TextModeCursor()
	{
		x = 0;
		y = 0;
	}
	int TextModeCursor::get_x()
	{
		return x;
	}
	int TextModeCursor::get_y()
	{
		return y;
	}
	void TextModeCursor::move(int x, int y)
	{
		this->x = x;
		this->y = y;
		uint16_t pos = y * 80 + x;
 
		outb(0x3D4, 0x0F);
		outb(0x3D5, (uint8_t) (pos & 0xFF));
		outb(0x3D4, 0x0E);
		outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
	}
	void TextModeCursor::moveY(int y)
	{
		this->y = y;
		move(x, y);
	}
	void TextModeCursor::moveX(int x)
	{
		this->x = x;
		move(x, y);
	}
	#pragma endregion
	#pragma region COLORS
	ConsoleColor Console::GetFG()
	{
		return fg;
	}
	ConsoleColor Console::GetBG()
	{
		return bg;
	}
	void Console::SetFG(ConsoleColor Color)
	{
		fg = Color;
	}
	void Console::SetBG(ConsoleColor Color)
	{
		bg = Color;
	}
	#pragma endregion
}