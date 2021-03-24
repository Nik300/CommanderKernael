//Copyright Nicola Leone Ciardi (C)2020
//This file has been licensed by GNU-GPL
//All rights reserved. (C)2020
//Only contributors are authorized to modify/redistribute
//these files

//See CONTRIBUTORS.TXT for more infos

#ifndef __COMMANDERKERNAEL_DRIVERS_TIMER_HPP
#define __COMMANDERKERNAEL_DRIVERS_TIMER_HPP
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
        TaskManager* TimerManager;
        class MultitaskingTimer: public driver{
        public:
            static uint32_t GetFromESP(){
                uint32_t result;
                __asm__ volatile("mov %%esp, %0" : "=a" (result));
                return result;
            }
            static void SetToESP(uint32_t data){
                __asm__ volatile("mov %0, %%esp" : : "a" (data));
            }
            static void TimerCallback(uint32_t* regs){
                //core::debug::send_string(SERIAL_PORT_A, intToASCII(((registers_t*)(regs->esp))->eip));
                //core::debug::send_string(SERIAL_PORT_A, "\n");
                /*core::debug::send_string(SERIAL_PORT_A, intToASCII((*regs)));
                core::debug::send_string(SERIAL_PORT_A, "\n");*/
                registers_t* r = TimerManager->Schedule(((registers_t*)*regs));
                *regs = (uint32_t)r;
                /*((registers_t*)*regs)->eax = r->eax;
                ((registers_t*)*regs)->ebx = r->ebx;
                ((registers_t*)*regs)->ecx = r->ecx;
                ((registers_t*)*regs)->edx = r->edx;
                ((registers_t*)*regs)->esi = r->esi;
                ((registers_t*)*regs)->edi = r->edi;
                ((registers_t*)*regs)->ebp = r->ebp;
                ((registers_t*)*regs)->int_no = r->int_no;
                ((registers_t*)*regs)->err_code = r->err_code;
                ((registers_t*)*regs)->cs = r->cs;
                ((registers_t*)*regs)->eflags = r->eflags;
                ((registers_t*)*regs)->useresp = r->useresp;
                ((registers_t*)*regs)->ss = r->ss;*/
		        //__asm__ volatile("sti");
                ((registers_t*)*regs)->eip = r->eip;
                //core::debug::send_string(SERIAL_PORT_A, intToASCII((*regs)));
                //core::debug::send_string(SERIAL_PORT_A, "\n");
                /*core::debug::send_string(SERIAL_PORT_A, intToASCII(((registers_t*)(regs->esp))->eip));
                core::debug::send_string(SERIAL_PORT_A, "\n");
                core::debug::send_string(SERIAL_PORT_A, "\n");*/
            }
            MultitaskingTimer(TaskManager* taskManager)
            :driver("Multitasking timer driver", 0, CommanderKernael::Drivers::MultitaskingTimer::TimerCallback)
            {
                TimerManager = taskManager;
            }
            MultitaskingTimer(TaskManager* taskManager, uint32_t freq)
            :driver("Multitasking timer driver", 0, CommanderKernael::Drivers::MultitaskingTimer::TimerCallback){
                TimerManager = taskManager;
                /* Get the PIT value: hardware clock at 1193180 Hz */
                uint32_t divisor = 1193180 / freq;
                uint8_t low  = (uint8_t)(divisor & 0xFF);
                uint8_t high = (uint8_t)( (divisor >> 8) & 0xFF);
                /* Send the command */
                port8bit* a = new port8bit(0x43);
                port8bit* b = new port8bit(0x40);
                a->write(0x36); /* Command port */
                b->write(low);
                b->write(high);
            }
        };
    }
}


#endif //__COMMANDERKERNAEL_DRIVERS_TIMER_HPP