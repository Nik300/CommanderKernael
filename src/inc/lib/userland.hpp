#ifndef __cplusplus
#error "This header is C++ only."
#endif

#include <stdint.h>
#include <stddef.h>
#include <std/stddecl.h>

#include <lib/heap.hpp>

namespace System::Userland
{
	using namespace System::Memory;

	extern Heap UserHeap;
	
}