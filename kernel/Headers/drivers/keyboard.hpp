//Copyright Nicola Leone Ciardi (C)2020
//This file has been licensed by GNU-GPL
//All rights reserved. (C)2020
//Only contributors are authorized to modify/redistribute
//these files

//See CONTRIBUTORS.TXT for more infos

#ifndef __COMMANDERKERNAEL_DRIVERS_KEYBOARD_KEYBOARD_HPP
#define __COMMANDERKERNAEL_DRIVERS_KEYBOARD_KEYBOARD_HPP
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

#include <libc/lock.hpp>
#include <libc/collections.hpp>

using namespace CommanderKernael::Drivers;
using namespace CommanderKernael::Drivers::stdio;
using namespace CommanderKernael::utilities::ASCII_UTIL;
using namespace CommanderKernael::utilities::memory;
using namespace assembly::ports;
using namespace assembly::types;
using namespace CommanderKernael::multitasking;
using namespace CommanderKernael::core;

using namespace System;
using namespace System::Collections::Generic;

namespace CommanderKernael{
    namespace Drivers{
        class keyboardDriver;
    }
}

namespace CommanderKernael{
    namespace keyboardSchemes{
        class keyboardScheme{
        friend class CommanderKernael::Drivers::keyboardDriver;
        private:
            List<char>* _chars = new List<char>();
            Console* console;
            char recorded_input;
            bool record_input = false;
            bool shift = false;
            bool enabled = false;
            int chars = 0;
            lock* ConsoleLock = new lock();
        public:
            String* _sc_names;
            String* _sc_chars;
            String* _sc_caps_chars;
            keyboardScheme(Console *console){
                this->console = console;
            }
            keyboardScheme(){
            }
            ~keyboardScheme(){}
            void printChar(uint8_t code){
				this->console->takeOwnership();
                if(this->shift) {
                    String _char = this->_sc_caps_chars[(int)code];
                    if (enabled) {
                        //print char
                        this->console->printf(_char);
                    }

                    if (!this->record_input) return;

                    //get input
                    recorded_input = _char[0];
                    _chars->Add(_char[0]);
                }
                else {
                    String _char = this->_sc_chars[(int)code];
                    if (enabled) {
                        //print char
                        this->console->printf(_char);
                    }

                    if (!this->record_input) return;

                    //get input
                    recorded_input = _char[0];
                    _chars->Add(_char[0]);
                }
            }
            void printN(){
                if (!this->record_input) return;
				this->console->takeOwnership();
                this->console->printf('\n');
                recorded_input = '\n';
            }
            void backspace(){
                if (!this->record_input) return;
            	if (_chars->Count <= 0) return;
            	console->printf_at(" ", console->cursor_x() - 1, console->cursor_y());
            	console->set_cursor_x(console->cursor_x() - 1);
                _chars->RemoveAt(_chars->Count - 1);
                recorded_input = '\0';
            	chars = _chars->Count;
            }
            void insert_shift(){
                this->shift = true;
            }
            void release_shift(){
                this->shift = false;
            }
            char read_key(bool output = true){
                enabled = output;
                console->takeOwnership();
                this->record_input = true;
                this->recorded_input = '\0';
                while(this->recorded_input == '\0');
                this->record_input = false;
                enabled = false;
                return recorded_input;
            }
            string read_line(bool output = true){
                ConsoleLock->Lock(CurrentThread);
                _chars->Clear();
                int i = 0;
                chars = 0;
                bool _continue = true;
                while(_continue){
                    char c = read_key(output);
                    if (chars != i) i = chars;
                    if (c == '\n') {
                        _continue = false;
                        if (i == 0){
                        }
                        break;
                    }
                    //_chars->Add(c);
                    i++;
                    chars++;
                }
				core::debug::send_string(SERIAL_PORT_A, (string)"Before last add\n");
                _chars->Add('\0');
				core::debug::send_string(SERIAL_PORT_A, (string)"Before to array\n");
                String str = _chars->ToArray();
				core::debug::send_string(SERIAL_PORT_A, (string)"Before creation of string\n");
                string _ret = "";
                if (str == NULL)
                    _ret = string("");
                else 
                    _ret = str;
				core::debug::send_string(SERIAL_PORT_A, (string)"Before unlock\n");
                ConsoleLock->Unlock();
				core::debug::send_string(SERIAL_PORT_A, (string)"Before Next()\n");
                Next();
				core::debug::send_string(SERIAL_PORT_A, (string)"Before return\n");
                return _ret;
			}
        };
        static CommanderKernael::keyboardSchemes::keyboardScheme* scelectedScheme;
    }
}

namespace CommanderKernael{
    namespace Drivers{
        class Key{
        public:
            string name;
            char KeyChar;
            Key(string name, char KeyChar)
            :name(name){
                this->KeyChar = KeyChar;
            }
        };
        namespace KeyboardDelegates{
            typedef void (*OnKeyDown)(Key* keyPressed);
            OnKeyDown* events = (OnKeyDown*)CommanderKernael::core::memory::allocateMemory(256);
            int counter = 0;
        }
        class keyboardDriver: public driver{
        public:
            static void AddOnKeyDownDelegate(KeyboardDelegates::OnKeyDown delegate){
                if (KeyboardDelegates::counter >= 256) return;
                KeyboardDelegates::events[KeyboardDelegates::counter++] = delegate;
            }
            static void keyboard_callback(uint32_t* regs)
            {
                __asm__ volatile("cli");
                port8bit kport = port8bit(0x60);
                uint8_t scancode = kport.read();
                if (CommanderKernael::keyboardSchemes::scelectedScheme->_sc_names[(scancode - 0x80)] == "LShift" || CommanderKernael::keyboardSchemes::scelectedScheme->_sc_names[(scancode - 0x80)] == "RShift") CommanderKernael::keyboardSchemes::scelectedScheme->release_shift();
                if (scancode > 57) return;
                if (scancode == ENTER) CommanderKernael::keyboardSchemes::scelectedScheme->printN();
                else if (scancode == BACKSPACE) CommanderKernael::keyboardSchemes::scelectedScheme->backspace();
                else if (scancode == ESC) return;
                else if (CommanderKernael::keyboardSchemes::scelectedScheme->_sc_names[(scancode)] == "Lctrl") return;
                else if (CommanderKernael::keyboardSchemes::scelectedScheme->_sc_names[(scancode)] == "LAlt") return;
                else if (CommanderKernael::keyboardSchemes::scelectedScheme->_sc_names[(scancode)] == "LShift" || CommanderKernael::keyboardSchemes::scelectedScheme->_sc_names[(scancode)] == "RShift") CommanderKernael::keyboardSchemes::scelectedScheme->insert_shift();
                else CommanderKernael::keyboardSchemes::scelectedScheme->printChar(scancode);

                for (int i = 0; i < KeyboardDelegates::counter; i++){
                    if (CommanderKernael::keyboardSchemes::scelectedScheme->shift)
                        KeyboardDelegates::events[i](new Key(CommanderKernael::keyboardSchemes::scelectedScheme->_sc_names[(scancode)], CommanderKernael::keyboardSchemes::scelectedScheme->_sc_caps_chars[(scancode)][0]));
                    else
                        KeyboardDelegates::events[i](new Key(CommanderKernael::keyboardSchemes::scelectedScheme->_sc_names[(scancode)], CommanderKernael::keyboardSchemes::scelectedScheme->_sc_chars[(scancode)][0]));
                }
                __asm__ volatile("sti");
            }
            keyboardDriver();
            ~keyboardDriver(){}
        };
    }
}

CommanderKernael::Drivers::keyboardDriver::keyboardDriver()
:driver("Keyboard Driver", 1, keyboardDriver::keyboard_callback)
{
}

#endif //__COMMANDERKERNAEL_DRIVERS_KEYBOARD_KEYBOARD_HPP
