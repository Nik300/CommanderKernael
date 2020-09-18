//Copyright Giovanni Masi (C)2020
//This file has been licensed by GNU-GPL
//All rights reserved. (C)2020
//Only contributors are authorized to modify/redistribute
//this files.

//See CONTRIBUTORS.TXT for more infos

#ifndef __COMMANDERKERNAEL_KEYBOARDSCHEMES_ITALIAN
#define __COMMANDERKERNAEL_KEYBOARDSCHEMES_ITALIAN

#include <Headers/drivers/keyboard.hpp>
#include <Headers/stdio.hpp>
#include <Headers/types.hpp>

using namespace CommanderKernael::Drivers;
using namespace CommanderKernael::Drivers::stdio;
using namespace assembly::ports;
using namespace assembly::types;

namespace CommanderKernael{
	namespace keyboardSchemes{
		class Italian: public CommanderKernael::keyboardSchemes::keyboardScheme{
		public:
			Italian(Console* console);
			~Italian(){}
			String names[58] = {
                		"", "Esc", "1", "2", "3", "4", "5", "6",
                		"7", "8", "9", "0", "'", "ì", "Backspace", "Tab", "Q", "W", "E",
                		"R", "T", "Y", "U", "I", "O", "P", "è", "+", "Enter", "Lctrl",
                		"A", "S", "D", "F", "G", "H", "J", "K", "L", "ò", "à", "ù",
                		"LShift", "<", "Z", "X", "C", "V", "B", "N", "M", ",", ".",
                		"-", "RShift", "Keypad *", "LAlt", "Spacebar"
			};
			String chars[58] = {
                        "", "", "1", "2", "3", "4", "5", "6",
                        "7", "8", "9", "0", "'", "ì", "", "", "q", "w", "e", "r", "t", "y",
                		"u", "i", "o", "p", "è", "+", "", "", "a", "s", "d", "f", "g",
                		"h", "j", "k", "l", "ò", "à", "ù", "", "<", "z", "x", "c", "v",
                		"b", "n", "m", ",", ".", "-", "", "", "", " "
			};
            String caps_chars[58] = {
                        "", "", "!", "\"", "£", "$", "%", "&",
                        "/", "(", ")", "=", "?", "^", "", "", "Q", "W", "E", "R", "T", "Y",
                		"U", "I", "O", "P", "é", "*", "", "", "A", "S", "D", "F", "G",
                		"H", "J", "K", "L", "ç", "°", "§", "", ">", "Z", "X", "C", "V",
                		"B", "N", "M", ";", ":", "_", "", "", "", " "
			};
		};
	}
}

CommanderKernael::keyboardSchemes::Italian::Italian(Console* console)
:CommanderKernael::keyboardSchemes::keyboardScheme(console){
	for (int i = 0; i < 58; i++){
		this->_sc_names[i] = this->names[i];
		this->_sc_chars[i] = this->chars[i];
		this->_sc_caps_chars[i] = this->caps_chars[i];
	}
}

#endif //__COMMANDERKERNAEL_KEYBOARDSCHEMES_ITALIAN
