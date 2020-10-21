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
#include <Headers/pci.hpp>

using namespace CommanderKernael;
using namespace core;
using namespace Drivers;
using namespace CommanderKernael::utilities::ASCII_UTIL;
using namespace CommanderKernael::core::HAL::supports::PCI;

namespace CommanderKernael{
    namespace main{
        class shell{
        private:
            Console console;
		    CommanderKernael::Drivers::keyboardDriver kdriver = CommanderKernael::Drivers::keyboardDriver();
            int shell_status;
            bool hang = false;
        public:
            shell(Console *_console, int status = 0){
				kdriver.activate();
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
                        console.println("CommanderKernael [ver: 0.3.1.0 Insider Alpha 3]\nDeveloped by: StellarSoft\nCore Developer: Nicola Leone Ciardi\nDeveloper: Giovanni Masi");
                    }
                    else if(string("help").compare(line)){
                        console.takeOwnership();
                        console.println("info: Sends infos about CommanderKernael");
                        console.println("help: Sends this message");
                        console.println("panic: Causes a kernel panic");
                        console.println("help: Sends this message");
                        console.println("clear: clears the screen");
                        console.println("dir: lists contents in the current directory");
                    }
                    else if(string("panic").compare(line)){
                        cpu::interrupts::call<2>();
                    }
                    else if(string("clear").compare(line)){
                        console.takeOwnership();
                        console.clear();
                    }
                    else if(string("dir").compare(line)){
                        console.takeOwnership();
                        console.println("FileSystems are not yet supported");
                    }
                    else if(string("keyboard\0").compare(line)){
                        console.takeOwnership();
                        console.printf("new keyboard scheme: ");
                        string kscheme = CommanderKernael::keyboardSchemes::scelectedScheme.read_line();
                        if (string("ita").compare(kscheme)){
                            console.takeOwnership();
                            CommanderKernael::keyboardSchemes::scelectedScheme = CommanderKernael::keyboardSchemes::Italian(&this->console);
                            console.takeOwnership();
                            console.println("keyboard scheme set up successfully to italian");
                            kdriver.deactivate();
                            kdriver.activate();
                        }
                        else if (string("eng").compare(kscheme)){
                            console.takeOwnership();
                            CommanderKernael::keyboardSchemes::scelectedScheme = CommanderKernael::keyboardSchemes::English(&this->console);
                            console.takeOwnership();
                            console.println("keyboard scheme set up successfully to english");
                            kdriver.deactivate();
                            kdriver.activate();
                        }
                    }
                    else if(string("pci").compare(line)){
                        PCIController controller = PCIController();
                        controller.SelectDrivers(console);
                        console.takeOwnership();
                    }
                    else if(string("\0").compare(line)){
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
