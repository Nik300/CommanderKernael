#ifndef __LIBC_SYSTEM_EXCEPTIONS_HPP
#define __LIBC_SYSTEM_EXCEPTIONS_HPP

#include <Headers/stdio.hpp>
#include <Headers/multitasking.hpp>

using namespace CommanderKernael::Drivers::stdio;
using namespace CommanderKernael::multitasking;

namespace CommanderKernael{
    namespace main{
        extern TaskManager* taskManager;
    }
}

namespace System{
    namespace Exceptions{
        class Exception{
        friend class ExceptionHandler;
        public:
            string Message;
            Exception(string text)
            :Message(text){ }
        };
        class ExceptionHandler{
        public:
            static void Throw(Exception exception){
                if (!CurrentThread->crashed){
                    CurrentThread->crashed = true;
                    CurrentThread->TState = ThreadState::Killed;
                    CurrentThread->eHandler(taskManager->currentTask, CurrentThread, exception.Message, "Thread error", "Exception Handler");
                }
            }
        };
        #pragma region General Exceptions
        class NotYetImplementedException: public Exception{
        public:
            NotYetImplementedException(): Exception("Feature not yet implemented."){}
        };
        class Breakpoint: public Exception{
        public:
            Breakpoint(): Exception("Breakpoint."){}
        };
        class ArgumentNullException: public Exception{
        public:
            ArgumentNullException(string argName): Exception("Parameter cannot be null. Parameter name: " + argName){}
        };
        class Debug: public Exception{
        public:
            Debug(): Exception("Debug."){}
        };
        class BadAlloc: public Exception{
        public:
            BadAlloc(): Exception("Bad allocation during program runtime."){}
        };
        class UnexpectedException: public Exception{
        public:
            UnexpectedException(): Exception("An unexpected exception occurred during program runtime."){}
        };
        class IndexOutOfRangeException: public Exception{
        public:
            IndexOutOfRangeException(): Exception("The required index was out of the range of the array."){}
        };
        class RuntimeException: public Exception{
        public:
            RuntimeException(): Exception("An error during runtime occurred."){}
        };
        class OverflowException: public Exception{
        public:
            OverflowException(): Exception("An overflow occurred during program runtime."){}
        };
        class UnderflowException: public Exception{
        public:
            UnderflowException(): Exception("An underflow occurred during program runtime."){}
        };
        class IllegalOperation: public Exception{
        public:
            IllegalOperation(): Exception("This application performed an illegal operation."){}
        };
        #pragma endregion
    }
}

#define throw(x) ExceptionHandler::Throw(x);

#endif //__LIBC_SYSTEM_EXCEPTIONS_HPP