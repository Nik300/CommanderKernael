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
		virtual int get_x() const = 0;
		virtual int get_y() const = 0;

	public:
		virtual void move(int x, int y) = 0;
		virtual void moveX(int x) = 0;
		virtual void moveY(int y) = 0;
		virtual void init() = 0;
		virtual void show() = 0;
		virtual void hide() = 0;
	};

	class TextModeCursor final: public Cursor
	{
	friend class ConsoleStream;
	private:
		int x;
		int y;
	private:
		int get_x() const override;
		int get_y() const override;
	private:
		TextModeCursor();
		~TextModeCursor();

	public:
		void move(int x, int y) override;
		void moveX(int x) override;
		void moveY(int y) override;
		void init() override;
		void show() override;
		void hide() override;
		void set_cur_size(uint8_t x_size, uint8_t y_size);
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
		static int WriteLine(ConsoleColor color, const char* text, ...);
		static int Write(const char* text, ...);
		static int Write(ConsoleColor color,const char* text, ...);
		static int WriteV(const char* text, va_list args);
		static const char* ReadLine();
		static void Clear();
		static void Clear(ConsoleColor fg, ConsoleColor bg);
		static void Init(void *io = stdio);
		static ConsoleColor GetFG();
		static ConsoleColor GetBG();
		static void SetFG(ConsoleColor Color);
		static void SetBG(ConsoleColor Color);
	};
}
#endif