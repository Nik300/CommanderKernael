#ifndef __LIBC_SYSTEM_LOCK_HPP
#define __LIBC_SYSTEM_LOCK_HPP

#include <Headers/multitasking.hpp>

using namespace CommanderKernael::multitasking;

namespace System{
    class lock{
    private:
        bool lock = false;
        Task* lockedTask = 0;
    public:
        void Lock(){
            Lock(CurrentThread);
        }
        void Lock(Task* lockTask){
            if (lockedTask == lockTask) return;
            if (lock) while(lock)
                if (lockedTask->TState != Running)
                    break;
            lock = true;
            lockedTask = lockTask;
        }
        void Unlock(){
            lock = false;
            lockedTask = 0;
            Next();
        }
    };
}

#endif //__LIBC_SYSTEM_LOCK_HPP