//Copyright Nicola Leone Ciardi (C)2020
//This file has been licensed by GNU-GPL
//All rights reserved. (C)2020
//Only contributors are authorized to modify/redistribute
//this files.

//See CONTRIBUTORS.TXT for more infos

#ifndef __COMMANDERKERNAEL_DRIVERS_DRIVER_HPP
#define __COMMANDERKERNAEL_DRIVERS_DRIVER_HPP

#include "Headers/types.hpp"
#include "Headers/stdio.hpp"
#include "Headers/core.hpp"

using namespace assembly::types;
using namespace CommanderKernael::Drivers::stdio;
using namespace CommanderKernael::core;

namespace CommanderKernael{
	namespace Drivers{
        class driver{
        private:
            uint8_t iRQN;
        public:
            String name = "notSet";
            isr_t handler;
            static void disabled(registers_t regs){
                return;
            }
            bool active = false;
            void activate(){
                cpu::iRQ::addIRQHandler(this->iRQN, this->handler, false);
                this->active = true;
            }
            void deactivate(){
                cpu::iRQ::addIRQHandler(this->iRQN, CommanderKernael::Drivers::driver::disabled, false);
                this->active = false;
            }
            void reset(){
                cpu::iRQ::addIRQHandler(this->iRQN, this->handler, false);
                this->active = true;
            }
            driver(String name, uint8_t iRQ, isr_t handler){
                this->name = name;
                this->iRQN = iRQ;
                this->handler = handler;
            }
            ~driver(){}
        };
	}
}

#endif //__COMMANDERKERNAEL_DRIVERS_DRIVER_HPP
