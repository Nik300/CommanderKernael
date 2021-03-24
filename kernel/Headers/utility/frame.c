#include <Headers/utility/types.h>
#include <Headers/utility/frame.h>
#include <Headers/utility/kalloc.h>

uint32_t stack_count = 0;
uint32_t* free_frames = NULL;
int32_t top_of_stack = -1;

uint32_t end_of_mem = 0;

uint32_t allocated_frames = 0;
uint32_t total_frames;

void init_frame_allocator(uint32_t system_frames) {
	total_frames = system_frames;
	if (free_frames != NULL) return;

	free_frames = (uint32_t*)e_kmalloc(system_frames * sizeof(uint32_t));
	stack_count = system_frames;
}

void alloc_frame(page* page, bool is_kernel, bool is_writeable) {
	if (page->frame != 0) return;
	allocated_frames++;
	uint32_t idx;
	if (top_of_stack >= 0) {
		idx = free_frames[top_of_stack];
		top_of_stack--;
	}
	else {
		if (end_of_mem >= total_frames) return;
		idx = end_of_mem;
		end_of_mem++;
	}
	page->present = 1;
	page->rw = (is_writeable) ? 1 : 0;
	page->user = (is_kernel) ? 0 : 1;
	page->frame = idx;
}

void free_frame(page* page) {
	top_of_stack++;
	if (((uint16_t)top_of_stack) >= stack_count) return;

	free_frames[top_of_stack] = page->frame;
	page->frame = 0x0;
	allocated_frames--;
}