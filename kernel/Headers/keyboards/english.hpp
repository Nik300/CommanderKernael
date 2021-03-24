//Copyright Giovanni Masi (C)2020
//This file has been licensed by GNU-GPL
//All rights reserved. (C)2020
//Only contributors are authorized to modify/redistribute
//these files

//See CONTRIBUTORS.TXT for more infos

#ifndef __COMMANDERKERNAEL_KEYBOARDSCHEMES_ENGLISH
#define __COMMANDERKERNAEL_KEYBOARDSCHEMES_ENGLISH

#include <Headers/drivers/keyboard.hpp>
#include <Headers/stdio.hpp>
#include <Headers/types.hpp>

using namespace CommanderKernael::Drivers;
using namespace CommanderKernael::Drivers::stdio;
using namespace assembly::ports;
using namespace assembly::types;

namespace CommanderKernael{
	namespace keyboardSchemes{
        class English: public CommanderKernael::keyboardSchemes::keyboardScheme{
        private:
            Console console;
        public:
            String _names[58] = {
                "ERROR", "Esc", "1", "2", "3", "4", "5", "6",
                "7", "8", "9", "0", "-", "=", "Backspace", "Tab", "Q", "W", "E",
                "R", "T", "Y", "U", "I", "O", "P", "[", "]", "Enter", "Lctrl",
                "A", "S", "D", "F", "G", "H", "J", "K", "L", ";", "'", "`",
                "LShift", "\\", "Z", "X", "C", "V", "B", "N", "M", ",", ".",
                "/", "RShift", "Keypad *", "LAlt", "Spacebar"
            };
            String _chars[58] = {
                "", "", "1", "2", "3", "4", "5", "6",
                "7", "8", "9", "0", "-", "=", "", "\t", "q", "w", "e", "r", "t", "y",
                "u", "i", "o", "p", "[", "]", "", "", "a", "s", "d", "f", "g",
                "h", "j", "k", "l", ";", "'", "`", "", "\\", "z", "x", "c", "v",
                "b", "n", "m", ",", ".", "/", "", "", "", " "
            };
            String _caps_chars[58] = {
                "", "", "!", "@", "#", "$", "%", "^",
                "&", "*", "(", ")", "_", "+", "", "\t", "Q", "W", "E", "R", "T", "Y",
                "U", "I", "O", "P", "[", "]", "", "", "A", "S", "D", "F", "G",
                "H", "J", "K", "L", ":", "", "~", "", "|", "Z", "X", "C", "V",
                "B", "N", "M", ",", ".", "/", "", "", "", " "
            };
            English(Console *console);
            ~English(){}
        };
	}
}

CommanderKernael::keyboardSchemes::English::English(Console *console)
:CommanderKernael::keyboardSchemes::keyboardScheme(console)
{
    this->_sc_chars = this->_chars;
    this->_sc_names = this->_names;
    this->_sc_caps_chars = this->_caps_chars;
}

#endif //__COMMANDERKERNAEL_KEYBOARDSCHEMES_ENGLISH
