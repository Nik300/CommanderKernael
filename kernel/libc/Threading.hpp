#ifndef __LIBC_SYSTEM_THREADING_HPP
#define __LIBC_SYSTEM_THREADING_HPP

#include <Headers/multitasking.hpp>

using namespace CommanderKernael::multitasking;

namespace System{
    namespace Threading{
        class Thread{
        public:
            Thread(){}
            ~Thread(){}
            static void Suspend(){
                CurrentThread->TState = ThreadState::Suspended;
            }
        };
    }
}

#endif