#include "Headers/core/cpu/interrupts/isr.h"
#include "Headers/stdio/stdio.h"
#include "Headers/utility/types.h"
#include "Headers/utility/ports.h"

uint8_t buffer[3];
uint8_t offset;
uint8_t buttons;
bool LeftButton = false;
int32_t _x, _y = 0;

void OnMouseChanged();

void OnMouseMove(int8_t xoffset, int8_t yoffset)
{
	_x += xoffset;
	if(_x < 0) _x = 0;
	_y += yoffset;
	if(_y < 0) _y = 0;
    OnMouseChanged();
}

static void Handle(uint32_t* r)
{
	uint8_t status = port_byte_in(0x64);
	if (!(status & 0x20))
		return;

	buffer[offset] = port_byte_in(0x60);
       
	offset = (offset + 1) % 3;

	if(offset == 0)
    {
		if(buffer[1] != 0 || buffer[2] != 0)
		{
			OnMouseMove((int8_t)buffer[1], -((int8_t)buffer[2]));
        }

		uint8_t i;

        for (uint8_t i = 0; i < 3; i++)
        {
            if((buffer[0] & (0x1<<i)) != (buttons & (0x1<<i)))
            {
                if ((uint8_t)(buttons & (0x1 << i)))
                    LeftButton = false;
                else
                    LeftButton = true;
            }
        }
        buttons = buffer[0];
	 }
}


void init_mouse()
{
	offset = 0;
    buttons = 0;
        
    port_byte_out(0x64, 0xA8);
    port_byte_out(0x64, 0x20);
    uint8_t status = port_byte_in(0x60) | 2;
    port_byte_out(0x64, 0x60);
    port_byte_out(0x60, status);

    port_byte_out(0x64, 0xD4);
    port_byte_out(0x60, 0xF4);
    port_byte_in(0x60);

	_x = 40;
	_y = 12;

	register_interrupt_handler(IRQ12, Handle);
}