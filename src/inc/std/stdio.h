#pragma once
#include <stdint.h>
#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif
extern void *stdio;
int print(const char* text);
int printf(const char* format, ...);
int printv(const char* format, va_list args);
#ifdef __cplusplus
}
#endif


#ifdef __cplusplus

namespace System::IO
{
	class Stream
	{
	public:
		virtual int read(char* buffer, int length) = 0;
		virtual int write(const char* buffer, int length) = 0;
	};


	class Cursor
	{
	friend class ConsoleStream;
	private:
		virtual int get_x() = 0;
		virtual int get_y() = 0;

	public:
		virtual void move(int x, int y) = 0;
		virtual void moveX(int x) = 0;
		virtual void moveY(int y) = 0;
	};

	class TextModeCursor final: public Cursor
	{
	friend class ConsoleStream;
	private:
		int x;
		int y;
	private:
		int get_x() override;
		int get_y() override;
	private:
		TextModeCursor();
		~TextModeCursor();

	public:
		void move(int x, int y) override;
		void moveX(int x) override;
		void moveY(int y) override;
	};

	class ConsoleStream final : public Stream
	{
	friend class Console;
	private:
		int columns;
		int rows;
		char* buffer;
		TextModeCursor cursor;
	private:
		ConsoleStream();
		~ConsoleStream();

	public:
		int read(char* buffer, int length) override;
		int write(const char* buffer, int length) override;
	};
	enum class ConsoleColor : uint8_t
	{
		Black = 0,
		Blue = 1,
		Green = 2,
		Cyan = 3,
		Red = 4,
		Magenta = 5,
		Brown = 6,
		LightGray = 7,
		DarkGray = 8,
		LightBlue = 9,
		LightGreen = 10,
		LightCyan = 11,
		LightRed = 12,
		LightMagenta = 13,
		Yellow = 14,
		White = 15
	};
	class Console final
	{
	private:
		static ConsoleStream console;
	public:
		static int WriteLine(const char* text, ...);
		static int Write(const char* text, ...);
		static int WriteV(const char* text, va_list args);
		static const char* ReadLine();
		static void Clear();
		static void Clear(ConsoleColor fg, ConsoleColor bg);
		static void Init(void *io = stdio);
	};
}
#endif