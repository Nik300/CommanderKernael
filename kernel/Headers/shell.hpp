//Copyright Nicola Leone Ciardi (C)2020
//This file has been licensed by GNU-GPL
//All rights reserved. (C)2020
//Only contributors are authorized to modify/redistribute
//these files

//See CONTRIBUTORS.TXT for more infos

#ifndef __COMMANDERKERNAEL_MAIN_SHELL_HPP
#define __COMMANDERKERNAEL_MAIN_SHELL_HPP

#include <Headers/environment.hpp>
#include <Headers/core.hpp>
#include <Headers/drivers/keyboard.hpp>
#include <Headers/keyboards/languages.hpp>
#include <Headers/util.hpp>
#include <Headers/stdio.hpp>
#include <Headers/pci.hpp>
#include <Headers/drivers/ata.hpp>
#include <Headers/drivers/PCI/VMWareSVGAII.hpp>
#include <Headers/Graphics/GraphicsManager.hpp>
#include <Headers/drivers/vga.hpp>
#include <Headers/drivers/mouse.hpp>
#include <Headers/FileSystems/CFS.hpp>
#include <Headers/FileSystems/FAT32.hpp>
#include <Headers/drivers/vesa.hpp>
#include <Headers/Graphics/VideoUtils.hpp>
#include <Headers/Graphics/FontsSys.hpp>
#include <Headers/Graphics/WindowConsole.hpp>
#include <Headers/FileSystems/VFS.hpp>

//libc libraries
#include <libc/collections.hpp>
#include <libc/math.hpp>
#include <libc/operators.hpp>
#include <libc/console.hpp>
#include <libc/Threading.hpp>
#include <libc/exceptions.hpp>
#include <libc/random.hpp>

//extensions
#include <Headers/extensions/GraphicsExtension.hpp>
#include <Headers/extensions/TestGraphics.hpp>

using namespace CommanderKernael;
using namespace CommanderKernael::core;
using namespace Drivers;
using namespace CommanderKernael::utilities::ASCII_UTIL;
using namespace CommanderKernael::core::HAL::supports::PCI;
using namespace CommanderKernael::Drivers::PCI::VMWareDrivers;
using namespace CommanderKernael::Graphics;
using namespace CommanderKernael::Drives::FileSystems;
using namespace CommanderKernael::core::memory;
using namespace CommanderKernael::utilities;

//libc namespaces
using namespace System::Collections::Generic;
using namespace System::Threading;
using namespace System::operators;
using namespace System::Exceptions;
using namespace System::Random;

#pragma region summary

//this region has the aim of explaining the content of this file
//shell.hpp is the shell of the kernel
//by now the commands are fixed into the kernel
//but as soon as i get MS-DOS FAT32 support i'll use external commands

#pragma endregion

namespace CommanderKernael{
    namespace main{
		TaskManager* taskManager = NULL;
        MultitaskingTimer* timer;
        VAL* vidModeDev = NULL;
        GraphicsManager* Manager = NULL;
        WindowContainer* kconsole;
        void StopTimer(){ timer->deactivate(); }
        void StartTimer(){ timer->activate(); }
        bool rendered = false;
        void UpdateG(){
        }
        void RegisterWindow(int container){
            core::debug::send_string(SERIAL_PORT_A, "adding container\n");
            WindowContainer* cont = (WindowContainer*)container;
            Manager->containers.Add(cont);
        }
        void WaitForRender(){
            VideoBios::VesaRendered = false;
            if(VideoBios::isVesaEnabled) while(!VideoBios::VesaRendered);
        }
        void UpdateConsole(){
            while (true){
                kconsole->refresh();
            }
        }
        void Update(){
            while (true)
            {
                if (vidModeDev == 0) continue;
                vidModeDev->Render();
                rendered = true;
            }
        }
        
        class shell{
        private:
            Console* console;
		    CommanderKernael::Drivers::keyboardDriver kdriver = CommanderKernael::Drivers::keyboardDriver();
            int shell_status;
            bool hang = false;

            VFS* fs = NULL;

            //video mode infos
            WindowContainer* kernelconsole;
            VAL* device;
            bool video = false;
            //CFS::CFS_FileSystem fs;

	    ConsoleColor mainShellBackColor = ConsoleColor::Black;
	    ConsoleColor mainShellForegroundColor = ConsoleColor::LightGray;
	    ConsoleColor mainShellInputColor = ConsoleColor::Cyan;
	    ConsoleColor mainShellReplyColor = ConsoleColor::Green;

        public:
            shell(Console *_console, int status = 0){
                console = _console;
                console->takeOwnership();
                this->log("Installing keyboard driver...");
				kdriver.activate();
                this->log("Keyboard installation completed");
                this->log("Keyboard scheme is: \"italian\"");
                shell_status = status;
                System::console = console;
                //if (!CFS::CFS_FileSystem::isDiskInitialized()) fs = CFS::CFS_FileSystem::initialize(90);
                //else fs = CFS::CFS_FileSystem();
            }
            ~shell(){
            }
            void log(string text) {
                console->takeOwnership();
                ConsoleColor oldF = console->getForegroundColor();
                console->setForegroundColor(ConsoleColor::Green);
                console->printf("[Shell] ");
                console->setForegroundColor(ConsoleColor::LightGray);
                console->println(text._iterator);
                console->setForegroundColor(oldF);
				core::debug::send_string(SERIAL_PORT_A, "[Shell] " + text);
				core::debug::send_string(SERIAL_PORT_A, "\n");
            }
            void start(){
                hang = true;
                console->println("Welcome to shell 3");
                while (hang){
                    RunSequence();
                }
            }
            void RunSequence(){
                console->takeOwnership();
                console->setForegroundColor(mainShellForegroundColor);
                console->setBackgroundColor(mainShellBackColor);
                console->printf("%kernael%> ");
                console->setForegroundColor(mainShellInputColor);
                RunCommand(CommanderKernael::keyboardSchemes::scelectedScheme->read_line(), this->console);
            }
            void RunCommand(string line, Console* console) {
				core::debug::send_string(SERIAL_PORT_A, (string)"[Shell] User \"");
                core::debug::send_string(SERIAL_PORT_A, Environment::SystemUser::selection::name);
                core::debug::send_string(SERIAL_PORT_A, "\" issued command ");
                core::debug::send_string(SERIAL_PORT_A, line);
				core::debug::send_string(SERIAL_PORT_A, "\n");
                console->takeOwnership();
                console->setForegroundColor(mainShellReplyColor);
                int comLen;
                string* comList = line.split(' ', &comLen);
                if (line == "info" || line == "about") {
                    console->takeOwnership();
                    console->setForegroundColor(ConsoleColor::Yellow);
                    console->println("CommanderKernael [ver: 0.7.0.1 Insider Alpha 1 Build 22.03.2021 - Covid times]\nDeveloped by: StellarSoft\nCore Developers: Nicola Leone Ciardi, Kevin Willibert Meerts\nDeveloper: Giovanni Masi\nMoral support: Will Hall\nSpecial thanks to the CosmosOS project that inspired us creating this!");
                }
                else if (comList[0] == "mkdir") {
                    if (comList[1].isEmpty()) {
                        console->println("Syntax error: mkdir (folder_name)");
                        return;
                    }
                    //fs.mkdir(comList[1]._iterator);
                }
                else if (comList[0] == "deldir") {
                }
                else if (comList[0] == "ata" && comList[1] == "write" && !comList[2].isEmpty()) {
                    CommanderKernael::Drivers::AdvancedTechnologyAttachement::Write(0, 1, (void*)comList[2]._iterator);
                }
                else if (string("help") == line) {
                    console->takeOwnership();
                    console->println("info: Shows infos about CommanderKernael");
                    console->println("help: Shows this message");
                    console->println("keyboard: Sets keyboard scheme");
                    console->println("gpus: Loads GUI framework (UNDER CONSTRUCTION!)");
                    console->println("panic: Causes a kernel panic");
                    console->println("clear: Clears the screen");
                    console->println("dir: Lists contents in the current directory");
                }
                else if (line == "CFS init") {
                }
                else if (line == "rand"){
                    Rand* rand = new Rand();
                    console->println(intToASCII(rand->RandInt(1, 100)));
                }
                else if (line == "randstr"){
                    Rand* rand = new Rand();
                    console->println(rand->RandString(10));
                }
                else if (comList[0] == "rand" && comLen == 3){
                    int min = StringToInt(comList[1]);
                    int max = StringToInt(comList[2]);
                    Rand* rand = new Rand();
                    console->println(intToASCII(rand->RandInt(min, max)));
                }
                else if (line == "FSinit") {
                    fs = new CFS::FileSystem();
                    fs->Initialize(30, false);
                    fs->CreateTable("Test table");
                    fs->CreatePartition("Test Part 1", 1, 2, "#1", 0, false, false);
                    fs->CreatePartition("Test Part 2", 5, 2, "#2", 0);;
                    fs->CreatePartition("Test Part 3", 8, 1, "#3", 0, true);
                    console->println(fs->GetDirName(1, "#1"));
                    fs->CreateDir(1, "#1/hello");
                    fs->CreateDir(1, "#1/hello/test");
                    //fs->CreateDir(1, "#1/hello/test/penis");
                    console->println(fs->GetDirName(1, "#1/hello"));
                }
                else if(comList[0] == "rootDir" && comLen == 2){
                    int num = StringToInt(comList[1]);
                    console->println(fs->GetRootDirName(num));
                }
                else if (comList[0] == "partname" && comLen == 2){
                    int num = StringToInt(comList[1]);
                    console->println(fs->PartName(num));
                }
                else if (comList[0] == "partid" && comLen == 2){
                    int num = StringToInt(comList[1]);
                    console->println(fs->GetPartID(num));
                }
                else if (comList[0] == "partflags" && comLen == 2){
                    int num = StringToInt(comList[1]);
                    console->printf_at("R/W", 0, console->cursor_y());
                    console->printf_at("Hidden\n", 5, console->cursor_y());
                    if (fs->PartIsRW(num))
                        console->printf_at("*", 1, console->cursor_y());
                    if (fs->PartIsHidden(num))
                        console->printf_at("*", 5, console->cursor_y());
                    console->println();
                }
                else if (comList[0] == "write" && comLen == 3){
                    int num = StringToInt(comList[1]);
                    fs->WriteToPartition(0, num, (void*)comList[2]._iterator, comList->lenght());
                    console->println("Done 1");
                }
                else if (comList[0] == "sizeof" && comLen == 2){
                    int i = StringToInt(comList[1]);
                    console->println(intSize(i));
                }
                else if (string("panic") == line) {
                    error::panic("Fake panic error", "Fatal Error", "Shell");
                }
                else if (comList[0] == "echo") {
                    line.remove(0, util::sizeofstr(comList[0]._iterator) + 1);
                    if (comList[1].isEmpty()) console->println();
                    else console->println(line);
                }
                else if (string("clear") == line) {
                    console->takeOwnership();
                    console->clear();
                }
                else if (line == "CFS test") {
                }
                else if (line == "mempos") {
                    this->log(intToASCII((uint32_t)&kernel_end));
                }
                else if (line == "CFS info") {
                }
                else if (string("dir") == line || line == "ld") {
                    /*int l;
                    CFS::CFS_Directory* dirs = fs.GetDirs(&l);
                    for (int i = 0; i < l; i++)
                        console->println("- " + fs.GetDirName(dirs[i]));*/
                }
                else if (line == "test") {
                    taskManager->AddTask(new ParametrizedTask<string, string, string>(1, [](string t1, string t2, string t3){
                        while (true){
                            System::console->takeOwnership();
                            System::console->println(t1 + t2 + t3);
                        }
                    }, new string("test"), "ciao ", "sono ", "Mario Draghi"));
                }
                else if (string("keyboard") == comList[0]) {
                    console->takeOwnership();
                    string kscheme = comList[1];
                    if (kscheme == "list") {
                        console->takeOwnership();
                        console->println("italian");
                        console->println("english");
                        console->println("french");
                        return;
                    }
                    else if (string("italian") == kscheme) {
                        console->takeOwnership();
                        delete CommanderKernael::Environment::SystemUser::selection::preferredKeyboardScheme;
                        CommanderKernael::Environment::SystemUser::selection::preferredKeyboardScheme = new CommanderKernael::keyboardSchemes::Italian(console);
                        CommanderKernael::keyboardSchemes::scelectedScheme = CommanderKernael::Environment::SystemUser::selection::preferredKeyboardScheme;
                        console->takeOwnership();
                    }
                    else if (string("english") == kscheme) {
                        console->takeOwnership();
                        delete CommanderKernael::Environment::SystemUser::selection::preferredKeyboardScheme;
                        CommanderKernael::Environment::SystemUser::selection::preferredKeyboardScheme = new CommanderKernael::keyboardSchemes::English(console);
                        CommanderKernael::keyboardSchemes::scelectedScheme = CommanderKernael::Environment::SystemUser::selection::preferredKeyboardScheme;
                        console->takeOwnership();
                    }
                    else if (kscheme == "german") {
                        console->takeOwnership();
                        console->println("Sorry kev, this is not a Nazi OS!\nIf I were in you I wouldn't suck dicks for 3$");
                        return;
                    }
                    else if (kscheme.isEmpty()) {
                        console->takeOwnership();
                        console->println("Syntax error.\nUsage: keyboard <keyboard_scheme>");
                        return;
                    }
                    else {
                        console->takeOwnership();
                        console->printf(kscheme._iterator);
                        console->println(" is not a valid keyboard scheme!\nUse command 'keyboard list' to list all supported keyboard schemes");
                        return;
                    }
                    console->printf("keyboard scheme successfully set to ");
                    console->println(kscheme._iterator);
                }
                else if (string("keyboard stop") == line) {
                    console->takeOwnership();
                    console->println("Stopping keyboard driver...");
                    kdriver.deactivate();
                    console->println("Keyboard driver stopped! Good luck writing! xD");
                }
                else if (string("keyboard current") == line) {
                    console->takeOwnership();
                    console->println("Could not retrieve current keyboard scheme!");
                }
                else if (line == "task test"){
                    //System::console->println("hello");
                    System::console = console;
                    taskManager->AddTask(new Task(1, [](){ while (true){ } }, new string("test_process")));
                }
                else if (comList[0] == "tasks"){
                    console->takeOwnership();
                    if (comList[1] == "list"){
                        console->takeOwnership();
                        console->println("PID   EIP     STACK SIZE  NAME    RUNNING");
                        for (int i = 0; i < taskManager->counter; i++){
                            console->printf(intToASCII(i));
                            console->printf("  ");
                            console->printf(intToASCII(taskManager->ActiveTasks[i]->state->eip));
                            console->printf("     ");
                            console->printf(intToASCII((uint32_t)taskManager->ActiveTasks[i]->stack->size/1024));
                            console->printf(" KiB");
                            console->printf("  ");
                            console->printf(*(taskManager->ActiveTasks[i]->name));
                            console->printf("    ");
                            if (taskManager->ActiveTasks[i]->TState == Running) console->println("*");
                            else console->println();
                        }
                    }
                    else if (comList[1] == "kill"){
                        if (comLen != 3){
                            console->println("Syntax error: tasks kill <PID>");
                            return;
                        }
                        int PID = StringToInt(comList[2]);
                        if (PID >= taskManager->counter){
                            console->println("Error: Process id out of range.");
                            return;
                        }
                        taskManager->ActiveTasks[PID]->TState = ThreadState::Killed;
                    }
                    else if (comList[1] == "suspend" && comLen == 2){
                        taskManager->ActiveTasks[taskManager->currentTask]->TState = Suspended;
                    }
                    else if (comList[1] == "suspend" && comLen == 3){
                        int PID = StringToInt(comList[2]);
                        if (PID >= taskManager->counter){
                            console->println("Error: Process id out of range.");
                            return;
                        }
                        taskManager->ActiveTasks[PID]->TState = Suspended;
                    }
                    else if (comList[1] == "resume" && comLen == 3){
                        int PID = StringToInt(comList[2]);
                        if (PID >= taskManager->counter){
                            console->println("Error: Process id out of range.");
                            return;
                        }
                        taskManager->ActiveTasks[PID]->TState = ThreadState::Running;
                    }
                    else{
                        console->println("Option invalid");
                    }
                }
                else if (line == "tlist"){
                    RunCommand("tasks list", console);
                    return;
                }
                else if(comList[0] == "tkill" && comLen == 2){
                    RunCommand("tasks kill " + comList[1], console);
                    return;
                }
                else if (string("ata load") == line) {
                    console->takeOwnership();
                    if (AdvancedTechnologyAttachement::loadATA())
                        console->println("ATA Drive loaded successfully");
                }
                else if (string("ata read") == line) {
                    console->takeOwnership();
                    uint8_t* dat = (uint8_t*)CommanderKernael::core::memory::allocateMemory(512);
                    AdvancedTechnologyAttachement::Read(0, 1, (void*)dat);
                    for (int i = 0; i < 512; i++)
                        console->printf((char)dat[i]);
                    console->println();
                }
                else if (string("pcil") == line) {
                    PCIController controller = PCIController();
                    controller.SelectDrivers(*console);
                    console->takeOwnership();
                }
                else if (string("gpus") == line) {

                    while (true);
                    console->takeOwnership();
                }
                else if (comList[0] == "colors" && comLen == 5){
                    mainShellForegroundColor = (ConsoleColor)StringToInt(comList[1]);
                    mainShellBackColor = (ConsoleColor)StringToInt(comList[2]);
                    mainShellInputColor = (ConsoleColor)StringToInt(comList[3]);
                    mainShellReplyColor = (ConsoleColor)StringToInt(comList[4]);
                    console->setBackgroundColor(mainShellBackColor);
                    console->clear();
                }
                else if (line == "colors reset"){
                    mainShellForegroundColor = ConsoleColor::LightGray;
                    mainShellBackColor = ConsoleColor::Black;
                    mainShellInputColor = ConsoleColor::Cyan;
                    mainShellReplyColor = ConsoleColor::Green;
                    console->setBackgroundColor(mainShellBackColor);
                    console->clear();
                }
                else if (string("split") == line) {
                    string str = CommanderKernael::keyboardSchemes::scelectedScheme->read_line();
                    int len;
                    string* res = str.split(' ', &len);
                    console->takeOwnership();
                    for (int i = 0; i < len; i++) console->println(res[i] == "a");
                }
                else if (string("memory count") == line) {
                    console->takeOwnership();
                    uint32_t sizekb = count_memory();

                    console->printf(intToASCII(sizekb));

                    console->println("MB of memory");
                }
                        else if (string("penis") == line) {
                            error::panic("\n      ___\n     //  7\n    (_,_/\\\n     \\    \\\n      \\    \\\n      _\\    \\__\n     (   \\     )\n      \\___\\___/", "Too Short Penis Error", "PENIS");
                        }
                        else if (line == "memory usage") {
                            console->printf("Using ");
                            console->printf((int)CommanderKernael::core::memory::memoryUsage());
                            console->printf("KB of ");
                            uint32_t sizekb = count_memory();
                            console->printf(intToASCII(sizekb));
                            console->println("MB of memory");
                        }
                        else if (line == "memory clear") {
                            console->println("Clearing used memory...");
                            uint8_t* dat = (uint8_t*)0x0000;
                            CommanderKernael::core::memory::clearMemory((void*)dat, (int)CommanderKernael::core::memory::memzone);
                            console->println("Memory has been cleared");
                        }
                        else if (line == "mouse click") {
                            while (true)
                            {
                                MouseDriver::enable();
                                console->println(MouseDriver::IsLeftPressed());
                            }
                        }
                        else if (comList[0] == "debug") {
                            for (int i = 1; i < comLen; i++) {
                                core::debug::send_string(SERIAL_PORT_A, comList[i]);
                                core::debug::send(SERIAL_PORT_A, ' ');
                            }
                            core::debug::send(SERIAL_PORT_A, '\n');
                        }
                        else if (string("vga enable") == line) {
                            VAL* VGADrv = new VideoGraphicsArray();
                            MouseDriver::enable();
                            VGADrv->SetMode(320, 200, 8);
                            WindowContainer* desktop = new WindowContainer(0, 0, 320, 200, VGADrv, 0x0f);
                            WindowContainer* wind1 = new WindowContainer(3, 3, 30, 20, VGADrv, 20);
                            while (true) {
                                desktop->refresh();
                                wind1->refresh();
                                VGADrv->RenderMouse(MouseDriver::GetX(), MouseDriver::GetY());
                                VGADrv->Render();
                            }
                            delete VGADrv;
                            console->takeOwnership();
                        }
                        else if (line == "crash"){
                            throw(Debug());
                        }
                        else if (line == "exit video"){
                            vidModeDev->Disable();
                            this->console = new Console();
                            System::console = this->console;
                            CommanderKernael::Environment::SystemUser::selection::preferredKeyboardScheme = new CommanderKernael::keyboardSchemes::Italian(this->console);
                            CommanderKernael::keyboardSchemes::scelectedScheme = CommanderKernael::Environment::SystemUser::selection::preferredKeyboardScheme;
                        }
                        else if (comList[0] == "vesa" && comList[1] == "enable") {
                            if (comLen != 5){
                                console->println("syntax error: vesa enable <width> <height> <color depth>");
                                return;
                            }

                            int width = StringToInt(comList[2]);
                            int height = StringToInt(comList[3]);
                            int depth = StringToInt(comList[4]);

                            //console->println(width);
                            //console->println(height);
                            //console->println(depth);

                            
                            Drivers::VideoBios::Vesa* vesadrv = new Drivers::VideoBios::Vesa();
                            MouseDriver::enable(width, height, vesadrv);
                            if (!vesadrv->SetMode(width, height, depth)) {
                                if(Drivers::VideoBios::isVesaEnabled)
                                {
                                    console->println("Vesa has already been enabled!\nDisable it first, or edit the screen resolution.");
                                    return;
                                }
                                console->println("Selected video resolution isn't available");
                                return;
                            }
                            //WindowContainer* desktop = new WindowContainer(0, 0, 800, 600, vesadrv, videoutils::GetRGB(102, 107, 103));
                            //WindowContainer* wind1 = new WindowContainer(3, 3, 30, 20, vesadrv, videoutils::GetRGB(10, 100, 50))

                            //taskManager->AddTask(new Task(1, mouseServices::RenderMouse, new string("Mouse_service")));
                            vidModeDev = vesadrv;

                            main::Manager = new GraphicsManager();
                            
                            //WindowContainer* container = new WindowContainer(0, 0, width, height, vesadrv, videoutils::GetRGB(0, 0, 0));
                            //kconsole = container;
                            
                            //taskManager->AddTask(new Task(4, UpdateConsole, new string("VESA_console_updater")));
							
                            main::Manager->RenderDone = false;

                            vesadrv->EnableThreadedRender();

					        //CommanderKernael::main::taskManager->AddTask(new Task(4, [](){
						    //    main::Manager->Update(vidModeDev);
					        //}, new string("GraphicsManager")));

                            kconsole = new WindowContainer(0, 0, 0, 0, NULL, 0);
                            static bool done = false;

                            CommanderKernael::main::taskManager->AddTask(new Task(4, [](){
                                System::Extensions::GraphicsExtension gext = System::Extensions::GraphicsExtension();
                                *kconsole = *(gext.SetupWindow(vidModeDev));
                                done = true;
                                gext.main(kconsole, NULL);
                            }, new string("GraphicsExtension")));
                            //while (true);

                            while(!done);

                            WindowConsole* c = new WindowConsole(kconsole);

                            debug::send_string(SERIAL_PORT_A, "[shell] initializing keyboard with vesa console...\n");
                            delete CommanderKernael::Environment::SystemUser::selection::preferredKeyboardScheme;
                            CommanderKernael::Environment::SystemUser::selection::preferredKeyboardScheme = new CommanderKernael::keyboardSchemes::Italian(c);
                            CommanderKernael::keyboardSchemes::scelectedScheme = CommanderKernael::Environment::SystemUser::selection::preferredKeyboardScheme;
                            debug::send_string(SERIAL_PORT_A, "[shell] Complete\n");
                            timer->activate();

                            this->console = c;
                            System::console = c;

                            System::console->setBackgroundColor(ConsoleColor::Blue);

                            console->takeOwnership();
                        }
                        else if (line == "shutdown") {
                            console->takeOwnership();
                            console->println("Shutting down...");
                            power::APM::shutdown();
                        }
                        else if (line == "suspend") {
                            console->takeOwnership();
                            console->println("Suspending...");
                            power::APM::suspend();
                            console->println("Suspended");
                        }
                        else if (line == "APM check") {
                            console->println(power::APM::Check());
                        }
                        else if (line == "reboot") {
                            console->takeOwnership();
                            console->println("Rebooting...");
                            cpu::functions::reboot("\0");
                        }
                        else if (line.isEmpty()) {
                        }
                        else {
                            console->takeOwnership();
                            console->setForegroundColor(ConsoleColor::LightRed);
                            console->println("Command not found!");
                            console->setForegroundColor(ConsoleColor::Cyan);
                            console->println("You may use command 'help' for a list of commands.");
                        }
                        console->takeOwnership();
                        //line.destroy();
            }
        };
    }
}

#endif //__COMMANDERKERNAEL_MAIN_SHELL_HPP
