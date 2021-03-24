//Copyright Nicola Leone Ciardi (C)2020
//This file has been licensed by GNU-GPL
//All rights reserved. (C)2020
//Only contributors are authorized to modify/redistribute
//these files

//See CONTRIBUTORS.TXT for more infos

#ifndef __COMMANDERKERNAEL_DRIVERS_CSSOCKET_HPP
#define __COMMANDERKERNAEL_DRIVERS_CSSOCKET_HPP
#define SC_MAX 57
#define BACKSPACE 0x0E
#define ENTER 0x1C
#define ESC 0x01

#include <Headers/environment.hpp>
#include <Headers/drivers/driver.hpp>
#include <Headers/stdio.hpp>
#include <Headers/types.hpp>
#include <Headers/core.hpp>
#include <Headers/ports.hpp>
#include <Headers/util.hpp>
#include <Headers/multitasking.hpp>

using namespace CommanderKernael::Drivers;
using namespace CommanderKernael::Drivers::stdio;
using namespace CommanderKernael::utilities::ASCII_UTIL;
using namespace CommanderKernael::utilities::memory;
using namespace assembly::ports;
using namespace assembly::types;
using namespace CommanderKernael::core;
using namespace CommanderKernael::multitasking;

namespace CommanderKernael{
    namespace Drivers{
        class CSSocket: public driver{
        public:
            static void TimerCallback(registers_t regs){
                CommanderKernael::core::cpu::GDTCodeSegment = regs.cs;
            }
            CSSocket()
            :driver("CSSocket", 0, CommanderKernael::Drivers::MultitaskingTimer::TimerCallback)
            {
            }
        };
    }
}


#endif //__COMMANDERKERNAEL_DRIVERS_CSSOCKET_HPP