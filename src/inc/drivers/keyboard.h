#pragma once
#ifdef __cplusplus
extern "C" {
#endif

const char* read_line();
//void scanf(const char* format, ...);

#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
#include <std/stdio.h>

#include <lib/driver.hpp>
#include <lib/memory.h>

#include <kernel.h>

namespace System::HAL
{
	class KeyboardDriver final : public InterruptDriver
	{
	private:
		handler_t handler;
		bool enabled;
		static uint8_t *buffer;
		static size_t buffer_size;

	public:
		KeyboardDriver();
		~KeyboardDriver();
	public:
		void Init() override;
		void Activate() override;
		void Deactivate() override;
		void Reset() override;
	public:
		static char **layout;
		static uint8_t *layout_shift;
		static uint8_t *layout_altgr;
	};
}

#endif
