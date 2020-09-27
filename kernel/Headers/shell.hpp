//Copyright Nicola Leone Ciardi (C)2020
//This file has been licensed by GNU-GPL
//All rights reserved. (C)2020
//Only contributors are authorized to modify/redistribute
//this files.

//See CONTRIBUTORS.TXT for more infos

#include <Headers/environment.hpp>
#include <Headers/core.hpp>
#include <Headers/drivers/keyboard.hpp>
#include <Headers/keyboards/languages.hpp>
#include <Headers/util.hpp>
#include <Headers/stdio.hpp>

using namespace CommanderKernael;
using namespace core;
using namespace Drivers;
using namespace CommanderKernael::utilities::ASCII_UTIL;

namespace CommanderKernael{
    namespace main{
        class shell{
        private:
            Console console;
            int shell_status;
            bool hang = false;
        public:
            shell(Console *_console, int status = 0){
                console = *_console;
                console.takeOwnership();
                shell_status = status;
            }
            ~shell(){
            }
            void start(){
                hang = true;
                while (hang){
                    console.takeOwnership();
                    console.printf("%kernel%> ");
                    string line = CommanderKernael::keyboardSchemes::scelectedScheme.read_line();
                    string command = string("info");
                    if (command.compare(line)){
                        console.takeOwnership();
                        console.println("CommanderKernael [ver: 0.2.3.1 Insider Alpha 10]\nDeveloped by: StellarSoft\nCore Developer: Nicola Leone Ciardi\nDeveloper: Giovanni Masi\nRompipalle di fiducia: Stefano Manca");
                    }
                    else if(string("help").compare(line)){
                        console.takeOwnership();
                        console.println("info: Sends infos about CommanderKernael");
                        console.println("help: Sends this message");
                        console.println("panic: Causes a kernel panic");
                        console.println("help: Sends this message");
                    }
                    else if(string("panic\0").compare(line)){
                        cpu::interrupts::call<2>();
                    }
                    else if(string("clear").compare(line)){
                        console.takeOwnership();
                        console.clear();
                    }
                    else if(string("\n").compare(line)){
                    }
                    else{
                        console.takeOwnership();
                        console.println("Command not found!\nYou may use command 'help' for a list of commands.");
                    }
                    memset(0x00, line._iterator, sizeof(line._iterator));
                }
            }
        };
    }
}
