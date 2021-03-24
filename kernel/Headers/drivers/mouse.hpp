#ifndef __COMMANDERKERNAEL_DRIVERS_MOUSE_HPP
#define __COMMANDERKERNAEL_DRIVERS_MOUSE_HPP

#include <Headers/environment.hpp>
#include <Headers/drivers/driver.hpp>
#include <Headers/stdio.hpp>
#include <Headers/types.hpp>
#include <Headers/core.hpp>
#include <Headers/ports.hpp>
#include <Headers/Graphics/VideoAbstractionLayer.hpp>
#include <Headers/util.hpp>
#include <Headers/drivers/vesa.hpp>

using namespace CommanderKernael::Drivers;
using namespace CommanderKernael::Drivers::stdio;
using namespace CommanderKernael::utilities::ASCII_UTIL;
using namespace CommanderKernael::utilities::memory;
using namespace assembly::ports;
using namespace assembly::types;
using namespace CommanderKernael::core;

extern "C"
{
    extern void init_mouse();
    extern uint8_t buffer[3];
    extern uint8_t offset;
    extern uint8_t buttons;
    extern int32_t _x, _y;
    bool limit;
    int mw;
    int mh;
    extern bool LeftButton;
}

namespace CommanderKernael{
    namespace main{
        extern TaskManager* taskManager;
        extern VAL* vidModeDev;
    }
	namespace Drivers{
        namespace mouseServices{
            static bool videoEnabled = false;
            static VAL* device;
        }

		namespace MouseDriver{
            static void enable(){
                init_mouse();
            }
            static void enable(int maxWidth, int maxHeight, VAL* dev) {
                CommanderKernael::Drivers::mouseServices::videoEnabled = true;
                CommanderKernael::Drivers::mouseServices::device = dev;
                limit = true;
                mw = maxWidth;
                mh = maxHeight;
                enable();
            }
            static int32_t GetX(){
                if (limit && _x > mw) _x = mw;
                if (_x < 0) _x = 0;
                return _x;
            }
            static int32_t GetY(){
                if (limit && _y > mh) _y = mh;
                if (_y < 0) _y = 0;
                return _y;
            }
            static bool IsLeftPressed(){
                return LeftButton;
            }
		}

        namespace mouseServices{
            void RenderMouse(){
                while (true){
                    if (CommanderKernael::main::vidModeDev != 0){
                        CommanderKernael::main::vidModeDev->RenderMouse(CommanderKernael::Drivers::MouseDriver::GetX(), CommanderKernael::Drivers::MouseDriver::GetY(), videoutils::GetRGB(255, 255, 255), videoutils::GetRGB(10, 10, 10));
                    }
                }
            }
        }
	}
}

extern "C" void OnMouseChanged(){
    core::cpu::interrupts::irq_ack(44);
}

#endif //__COMMANDERKERNAEL_DRIVERS_MOUSE_HPP