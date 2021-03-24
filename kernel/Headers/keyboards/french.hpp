//Copyright Giovanni Masi (C)2020
//This file has been licensed by GNU-GPL
//All rights reserved. (C)2020
//Only contributors are authorized to modify/redistribute
//these files

//See CONTRIBUTORS.TXT for more infos

#ifndef __COMMANDERKERNAEL_KEYBOARDSCHEMES_FRENCH
#define __COMMANDERKERNAEL_KEYBOARDSCHEMES_FRENCH

#include <Headers/drivers/keyboard.hpp>
#include <Headers/stdio.hpp>
#include <Headers/types.hpp>

using namespace CommanderKernael::Drivers;
using namespace CommanderKernael::Drivers::stdio;
using namespace assembly::ports;
using namespace assembly::types;

namespace CommanderKernael{
	namespace keyboardSchemes{
		class French: public CommanderKernael::keyboardSchemes::keyboardScheme{
		public:
			French(Console* console);
			~French(){}
			String names[58] = {
                		"", "Échap", "1", "2", "3", "4", "5", "6",
                		"7", "8", "9", "0", "°", "+", "Backspace", "Tab", "A", "Z", "E",
                		"R", "T", "Y", "U", "I", "O", "P", "è", "+", "Enter", "Lctrl",
                		"A", "S", "D", "F", "G", "H", "J", "K", "L", "ò", "à", "ù",
                		"LShift", "<", "Z", "X", "C", "V", "B", "N", "M", ",", ".",
                		"-", "RShift", "Keypad *", "LAlt", "Spacebar"
			};
			String chars[58] = {
                        "", "", "1", "2", "3", "4", "5", "6",
                        "7", "8", "9", "0", "'", "ì", "", "", "Q", "W", "E", "R", "T", "Y",
                		"U", "I", "O", "P", "è", "+", "", "", "A", "S", "D", "F", "G",
                		"H", "J", "K", "L", "ò", "à", "ù", "", "<", "Z", "X", "C", "V",
                		"B", "N", "M", ",", ".", "-", "", "", "", " "
			};
		};
	}
}

CommanderKernael::keyboardSchemes::French::French(Console* console)
:CommanderKernael::keyboardSchemes::keyboardScheme(console){
    this->_sc_chars = this->_chars;
    this->_sc_names = this->_names;
    this->_sc_caps_chars = this->_caps_chars;
}

#endif //__COMMANDERKERNAEL_KEYBOARDSCHEMES_FRENCH
