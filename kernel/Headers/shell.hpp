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
<<<<<<< HEAD
#include <Headers/drivers/PCI/VMWareSVGAII.hpp>
=======
>>>>>>> 21ea778aacfb419a58f37175bff031c5c2721ccf

using namespace CommanderKernael;
using namespace core;
using namespace Drivers;
using namespace CommanderKernael::utilities::ASCII_UTIL;
using namespace CommanderKernael::core::HAL::supports::PCI;
<<<<<<< HEAD
using namespace CommanderKernael::Drivers::PCI::VMWareDrivers;
=======
>>>>>>> 21ea778aacfb419a58f37175bff031c5c2721ccf

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
<<<<<<< HEAD
                        console.println("CommanderKernael [ver: 0.4.2.1 Insider Alpha 1 Build 24.10.2020]\nDeveloped by: StellarSoft\nCore Developer: Nicola Leone Ciardi\nDeveloper: Giovanni Masi");
=======
                        console.println("CommanderKernael [ver: 0.3.1.0 Insider Alpha 3]\nDeveloped by: StellarSoft\nCore Developer: Nicola Leone Ciardi\nDeveloper: Giovanni Masi");
>>>>>>> 21ea778aacfb419a58f37175bff031c5c2721ccf
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
<<<<<<< HEAD
                    else if(string("keyboard").compare(line)){
=======
                    else if(string("keyboard\0").compare(line)){
>>>>>>> 21ea778aacfb419a58f37175bff031c5c2721ccf
                        console.takeOwnership();
                        console.printf("new keyboard scheme: ");
                        string kscheme = CommanderKernael::keyboardSchemes::scelectedScheme.read_line();
                        if (string("ita").compare(kscheme)){
                            console.takeOwnership();
<<<<<<< HEAD
				            CommanderKernael::Environment::SystemUser::selection::preferredKeyboardScheme = CommanderKernael::keyboardSchemes::Italian(&this->console);
				            CommanderKernael::keyboardSchemes::scelectedScheme = CommanderKernael::Environment::SystemUser::selection::preferredKeyboardScheme;
                            console.takeOwnership();
                            console.println("keyboard scheme set up successfully to italian");
                        }
                        else if (string("eng").compare(kscheme)){
                            console.takeOwnership();
				            CommanderKernael::Environment::SystemUser::selection::preferredKeyboardScheme = CommanderKernael::keyboardSchemes::English(&this->console);
				            CommanderKernael::keyboardSchemes::scelectedScheme = CommanderKernael::Environment::SystemUser::selection::preferredKeyboardScheme;
                            console.takeOwnership();
                            console.println("keyboard scheme set up successfully to english");
                        }
                    }
                    else if(string("pcil").compare(line)){
=======
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
>>>>>>> 21ea778aacfb419a58f37175bff031c5c2721ccf
                        PCIController controller = PCIController();
                        controller.SelectDrivers(console);
                        console.takeOwnership();
                    }
<<<<<<< HEAD
                    else if(string("gpus").compare(line)){
                        VMWareSVGAII svga = VMWareSVGAII();
                        svga.SetMode(1920, 1080, 32);
                        int x = 0;
                        int y = 0;
                        int width = svga.width;
                        int height = svga.height;
                        uint32_t xTarget = (x + width);
                        uint32_t yTarget = (y + height);
                        for (uint32_t xTmp = x; xTmp < xTarget; xTmp++)
                        {
                            for (uint32_t yTmp = y; yTmp < yTarget; yTmp++)
                            {
                                svga.SetPixel(xTmp, yTmp, svga.Color(0, 0, 100));
                            }
                        }
                        svga.Render();
                        while(true);
                        console.takeOwnership();
                    }
                    else if(string("\n").compare(line)){
=======
                    else if(string("\0").compare(line)){
>>>>>>> 21ea778aacfb419a58f37175bff031c5c2721ccf
                    }
                    else{
                        console.takeOwnership();
                        console.println("Command not found!\nYou may use command 'help' for a list of commands.");
                    }
                    console.takeOwnership();
                    memset(0x00, line._iterator, sizeof(line._iterator));
                }
            }
        };
    }
}
