#ifndef __cplusplus
#error C++ is required to include this header
#endif

#include <stdint.h>
#include <stddef.h>

#include <std/cstring.h>
#include <std/stdio.h>

#include <lib/idt.hpp>

#include <kernel.h>

namespace System::HAL
{
	using namespace System::IO;

	class Driver
	{
	public:
		virtual void Init() = 0;
		virtual void Activate() = 0;
		virtual void Deactivate() = 0;
		virtual void Reset() = 0;
	};
	class InterruptDriver : public Driver
	{
	private:
		uint8_t irq;
		handler_t handler;

	public:
		virtual void Init() override;
		void Activate() override;
		void Deactivate() override;
		void Reset() override;
		void Fire();
	public:
		InterruptDriver(uint8_t irq, handler_t handler);
		~InterruptDriver();
	};

	class DriverManager final
	{
	private:
		Driver **loaded_drivers;
		size_t drivers_count;

	public:
		DriverManager();
		~DriverManager();

	public:
		Driver* &Register(Driver *driver);
		bool Unregister(int index);

	public:
		Driver* &operator [](uint8_t index);
		const Driver* &operator [](uint8_t index) const;
	};
}