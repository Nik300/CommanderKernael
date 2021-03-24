#ifndef __COMMANDERKERNAEL_UTILITIES_PAGING_HPP
#define __COMMANDERKERNAEL_UTILITIES_PAGING_HPP

#include <Headers/ports.hpp>
#include <Headers/stdio.hpp>

extern "C" {
	extern multiboot_header_t* mbootdat;
}

using namespace CommanderKernael::Drivers::stdio;
using namespace assembly::types;

extern "C" {
	void initialize_paging(uint32_t total_frames, uint32_t ident_addr, uint32_t ident_len);
	extern int kernel_start;
	extern int kernel_end;
}
namespace CommanderKernael {
	namespace utilities {
		namespace memory {
			void StartPaging() {
				int higertables = (mbootdat->mem_upper * 1024) / 4096;
				int lowertables = 256;

				uint32_t totalpages = lowertables + higertables;

				initialize_paging(totalpages, 0x7A8000, 80*30);
			}
		}
	}
}

#endif //__COMMANDERKERNAEL_UTILITIES_PAGING_HPP