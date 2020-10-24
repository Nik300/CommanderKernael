//Copyright Nicola Leone Ciardi (C)2020
//This file has been licensed by GNU-GPL
//All rights reserved. (C)2020
//Only contributors are authorized to modify/redistribute
//this files.

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

using namespace CommanderKernael::Drivers;
using namespace CommanderKernael::Drivers::stdio;
using namespace CommanderKernael::utilities::ASCII_UTIL;
using namespace CommanderKernael::utilities::memory;
using namespace assembly::ports;
using namespace assembly::types;
using namespace CommanderKernael::core;

namespace CommanderKernael{
    namespace keyboardSchemes{
        class keyboardScheme{
        private:
            Console console;
            char recorded_input;
            string recorded_string = string("");
            bool record_input = false;
            bool shift = false;
            bool enabled = false;
            int chars = 0;
        public:
            String _sc_names[58];
            String _sc_chars[58];
            String _sc_caps_chars[58];
            keyboardScheme(Console *console){
                this->console = *console;
            }
            keyboardScheme(){
                memset('\0', recorded_string._iterator, 20);
            }
            ~keyboardScheme(){}
            void printChar(uint8_t code){
				this->console.takeOwnership();
                if(this->shift) {
                    if (!enabled) return;
                    //print char
                    String _char = this->_sc_caps_chars[(int)code];
                    this->console.printf(_char);

                    if (!this->record_input) return;

                    //get input
                    recorded_input = _char[0];
                    recorded_string._iterator += _char[0];
                    this->record_input = false;
                }
                else {
                    if (!enabled) return;
                    //print char
                    String _char = this->_sc_chars[(int)code];
                    this->console.printf(_char);

                    if (!this->record_input) return;

                    //get input
                    recorded_input = _char[0];
                    recorded_string._iterator += _char[0];
                    this->record_input = false;
                }
            }
            void printN(){
                if (!enabled) return;
				this->console.takeOwnership();
                this->console.printf('\n');
                recorded_input = '\n';
                if (this->record_input) this->record_input = false;
            }
            void backspace(){
            	if (chars == 0)return;
            	console.printf_at(" ", console.cursor_x() - 1, console.cursor_y());
            	console.set_cursor_x(console.cursor_x() - 1);
                recorded_string._iterator[chars] = '\0';
            	chars--;
            }
            void insert_shift(){
                this->shift = true;
            }
            void release_shift(){
                this->shift = false;
            }
            char input_char(bool output = true){
                enabled = output;
                console.takeOwnership();
                this->record_input = true;
                while(this->record_input);
                enabled = false;
                return recorded_input;
            }
            string read_line(bool output = true){
                int i = 0;
                chars = 0;
                bool _continue = true;
                String dyn_char_array;
                while(_continue){
                    char c = input_char(true);
                    if (chars != i) i = chars;
                    if (c == '\n') {
                        _continue = false;
                        if (i == 0){
                            recorded_string._iterator += '\0';
                        }
                        break;
                    };
                    memset(0x00, (dyn_char_array + (i + 1)), 1);
                    dyn_char_array[i] = c;
                    i++;
                    chars++;
                }
<<<<<<< HEAD
                string _ret = string(recorded_string._iterator);
=======
                string _ret = string("");
                memset('\0', _ret._iterator, i+2);
                memcpy(recorded_string._iterator, _ret._iterator, i);
                _ret.end_char = _ret._iterator[sizeof(_ret._iterator)];
                memset('\0', recorded_string._iterator, sizeof(_ret._iterator));
>>>>>>> 21ea778aacfb419a58f37175bff031c5c2721ccf
                recorded_string = string("");
                return _ret;
			}
        };
        static CommanderKernael::keyboardSchemes::keyboardScheme scelectedScheme;
    }
}

namespace CommanderKernael{
    namespace Drivers{
        class keyboardDriver: public driver{
        public:
            static void keyboard_callback(registers_t regs)
            {
                port8bit kport = port8bit(0x60);
                uint8_t scancode = kport.read();
                if (CommanderKernael::keyboardSchemes::scelectedScheme._sc_names[(scancode - 0x80)] == "LShift" || CommanderKernael::keyboardSchemes::scelectedScheme._sc_names[(scancode - 0x80)] == "RShift") CommanderKernael::keyboardSchemes::scelectedScheme.release_shift();
                if (scancode > 57) return;
                if (scancode == ENTER) CommanderKernael::keyboardSchemes::scelectedScheme.printN();
                else if (scancode == BACKSPACE) CommanderKernael::keyboardSchemes::scelectedScheme.backspace();
                else if (scancode == ESC) return;
                else if (CommanderKernael::keyboardSchemes::scelectedScheme._sc_names[(scancode)] == "Lctrl") return;
                else if (CommanderKernael::keyboardSchemes::scelectedScheme._sc_names[(scancode)] == "LAlt") return;
                else if (CommanderKernael::keyboardSchemes::scelectedScheme._sc_names[(scancode)] == "LShift" || CommanderKernael::keyboardSchemes::scelectedScheme._sc_names[(scancode)] == "RShift") CommanderKernael::keyboardSchemes::scelectedScheme.insert_shift();
                else CommanderKernael::keyboardSchemes::scelectedScheme.printChar(scancode);
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
