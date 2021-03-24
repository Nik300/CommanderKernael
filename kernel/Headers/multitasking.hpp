//Copyright Nicola Leone Ciardi (C)2020
//This file has been licensed by GNU-GPL
//All rights reserved. (C)2020
//Only contributors are authorized to modify/redistribute
//these files

//See CONTRIBUTORS.TXT for more infos

#ifndef __COMMANDERKERNAEL_MULTITASKING_HPP
#define __COMMANDERKERNAEL_MULTITASKING_HPP

#include <Headers/drivers/driver.hpp>
#include <Headers/stdio.hpp>
#include <Headers/types.hpp>
#include <Headers/core.hpp>

#include <libc/console.hpp>
#include <libc/null.hpp>

using namespace CommanderKernael;
using namespace CommanderKernael::Drivers;
using namespace CommanderKernael::utilities;
using namespace CommanderKernael::Drivers::stdio;
using namespace assembly::types;
using namespace assembly::ports;
using namespace CommanderKernael::core::memory;

extern "C" uint32_t gdt_code;

namespace CommanderKernael{
    namespace multitasking{
        void Next(){
            core::cpu::interrupts::call<32>();
        }
        enum ThreadState{
            Running,
            Stopped,
            Suspended,
            Killed,
        };
        class Task
        {
        friend class TaskManager;
        template<typename T1, typename T2, typename T3> friend class ParametrizedTask;
        private:
            const bool killable;
        public:
            bool crashed = false;
            MemoryBlock<uint8_t>* stack; //stack of the task
            registers_t* state;
            ThreadState TState = ThreadState::Running;

            typedef void (*errorHandler)(int pid, Task* task, string error, string type, string from);
            typedef void (*errorHandlerRedir)(int pid, Task* task, string error, string type, string from, Console* console);

            typedef void (*ThreadStart)();
            ThreadStart actual_entry;
            errorHandler eHandler = 0;
            errorHandlerRedir eRedir = 0;
            uint32_t StackPos;

            string* name;
            
            static void Entry();

            void Suspend();

            Task* childTasks[1024];

            Task(int stackSizeInKb, void entry(), string* name, errorHandler ErrorHandler = 0, errorHandlerRedir ErrorHandlerRedir = 0, bool killable = true)
            :killable(killable){
                childTasks[0] = NULL;
                
                stack = new MemoryBlock<uint8_t>((stackSizeInKb * 1024) + sizeof(registers_t));
                state = (registers_t*)(stack->address + stack->size - sizeof(registers_t));

                if (name->numberOf(' ') != 0)
                    this->name = new string("NONAME");
                else 
                    this->name = name;

                if (ErrorHandler)
                    this->eHandler = ErrorHandler;
                else
                    this->eHandler = GeneralErrorHandler;
                
                if (ErrorHandlerRedir)
                    this->eRedir = GeneralErrorHandler;
                else
                    this->eRedir = GeneralErrorHandler;
                //state->ds = 0;

                state->edi = 0;
                state->esi = 0;
                state->ebp = 0;

                state->ebx = 0;
                state->edx = 0;
                state->ecx = 0;
                state->eax = 0;

                state->int_no = 0;
                state->err_code = 0;

                actual_entry = entry;

                state->eip = (uint32_t)Task::Entry;
                StackPos = (uint32_t)state;
                state->cs = 0x8;
                state->eflags = 0x202;
                //state->useresp = ;
                //state->ss = ;

            }
            ~Task(){
                crashed = 0;
                //delete state;
                //delete stack;
                TState = (ThreadState)0;
                actual_entry = 0;
                eHandler = 0;
                eRedir = 0;
                StackPos = 0;
                name = NULL;
            }
            static void GeneralErrorHandler(int pid, Task* task, string error, string type, string from){
                __asm__ volatile("cli");
                System::console->takeOwnership();
                System::console->println(error);
                __asm__ volatile("sti");
            }
            static void GeneralErrorHandler(int pid, Task* task, string error, string type, string from, Console* console){
                console->println(error);
            }
        };

        Task* CurrentThread;
        
        template<typename T1, typename T2, typename T3> class ParametrizedTask: public Task{
        public:
            T1 param1;
            T2 param2;
            T3 param3;

            typedef void (*ParametrizedThreadStart)(T1, T2, T3);

            ParametrizedThreadStart ActualTask;

            ParametrizedTask(int stackSizeInKb, ParametrizedThreadStart entry, string* name, T1 param1, T2 param2, T3 param3, errorHandler ErrorHandler = 0, errorHandlerRedir ErrorHandlerRedir = 0, bool killable = true)
            : Task(stackSizeInKb, 
            [](){ ParametrizedTask* thread = (ParametrizedTask*)CurrentThread; thread->ActualTask(thread->param1, thread->param2, thread->param3); },
            name, ErrorHandler, ErrorHandlerRedir, killable){
                ActualTask = entry;
                this->param1 = param1;
                this->param2 = param2;
                this->param3 = param3;
            }
            
        };

        class TaskManager
        {
        public:
            Task** ActiveTasks;
            int counter;
            int currentTask;
            TaskManager(){
                counter = 0;
                currentTask = -1;
                ActiveTasks = (Task**)CommanderKernael::core::memory::allocateMemory(256*sizeof(Task*));
                for (int i = 0; i < 256; i++)
                    ActiveTasks[i] = 0;
            }
            ~TaskManager(){}
            bool AddTask(Task* task){
                if (counter >= 256) return false;
                ActiveTasks[counter++] = task;
                return true;
            }
            registers_t* Schedule(registers_t* currentState){
                if (counter <= 0)
                    return currentState;

                if (currentTask >= 0){
                    ActiveTasks[currentTask]->state = (registers_t*)currentState;
                    //ActiveTasks[currentTask]->state->eip = currentState->eip;
                }

                if (++currentTask >= counter)
                    currentTask = 0;

                if (!ActiveTasks[currentTask]->killable && ActiveTasks[currentTask]->TState == ThreadState::Killed && !ActiveTasks[currentTask]->crashed){
                    ActiveTasks[currentTask]->TState = ThreadState::Running;
                }
                
                if (ActiveTasks[currentTask]->TState == ThreadState::Killed || ActiveTasks[currentTask]->TState == ThreadState::Stopped){
                    ActiveTasks[currentTask]->~Task();
                    for (int i = 1; i < counter; i++)
                        ActiveTasks[currentTask + (i - 1)] = ActiveTasks[currentTask + i];
                    ActiveTasks[counter-1] = 0;
                    counter--;
                    currentTask--;
                    return Schedule(currentState);
                }
                else if (ActiveTasks[currentTask]->TState == ThreadState::Suspended){
                    return Schedule(currentState);
                }

                //ActiveTasks[currentTask]->state->eip = ActiveTasks[currentTask]->CodePos;
                //core::debug::send_string(SERIAL_PORT_A, intToASCII((uint32_t)ActiveTasks[currentTask]->state));
                //core::debug::send_string(SERIAL_PORT_A, "\n");

                CurrentThread = ActiveTasks[currentTask];
                
                return ActiveTasks[currentTask]->state;
            }
        };
        void Task::Entry(){
            CurrentThread->actual_entry();
            CurrentThread->TState = ThreadState::Stopped;
            while (true);
        }
        void Task::Suspend(){
            TState = Suspended;
            if(CurrentThread == this) while(TState == Suspended);
        }
    }
}

#endif //__COMMANDERKERNAEL_MULTITASKING_HPP