#ifndef __FRAME_H
#define __FRAME_H
#include <Headers/utility/types.h>
#include <Headers/utility/paging.h>

void init_frame_allocator(uint32_t system_frames);
void alloc_frame(page* page, bool is_kernel, bool is_writeable);
void free_frame(page* page);

#endif // __FRAME_H
