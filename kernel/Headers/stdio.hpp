#ifndef __COMMANDERKERNAEL_DRIVERS_STD_STDIO_HPP
#define __COMMANDERKERNAEL_DRIVERS_STD_STDIO_HPP

#include "Headers/types.hpp"

using namespace assembly::types;

#define NIL 0


namespace CommanderKernael {
	namespace core {
		namespace memory {
			void* allocateMemory(int size);
			void* allocateMemory(int size, void* data);
			void* allocateMemory(int size, bool force);
			void* allocateUnmanagedMemory(int size);
			void releaseMemory(void* content);
			void releaseMemory(void* content, int size);
			void clearMemory(void* data, int size);
		}
	}
	namespace utilities {
		namespace ASCII_UTIL {
			char* intToASCII(int number);
		}
		namespace memory {
			typedef char* content_t;
			typedef unsigned short* content_short_t;
			typedef unsigned int* content_int_t;
			void memcpy(content_t source, content_t destination, int lenght);
			void memset(uint8_t content, content_t destination, int lenght);
			void memcpy(content_short_t source, content_short_t destination, int lenght);
			void memset(uint16_t content, content_short_t destination, int lenght);
			void memcpy(content_int_t source, content_int_t destination, int lenght);
			void memset(uint32_t content, content_int_t destination, int lenght);
		}
	}
}

using namespace CommanderKernael::utilities::ASCII_UTIL;

extern "C" void clear_screen();
extern "C" void kprint_at(char*, int, int);
extern "C" void kprint(char*);
extern "C" int get_cursor_offset();
extern "C" void set_cursor_offset(int offset);
extern "C" int get_offset(int col, int row);
extern "C" int get_offset_row(int offset);
extern "C" int get_offset_col(int offset);
extern "C" void changeFColor(char ncolor);
extern "C" void changeBColor(char ncolor);
extern "C" void disable_cursor();
extern "C" {
	extern char color;
}

uint32_t ownership = 0x0000080;

namespace CommanderKernael{
	namespace Drivers{
		namespace stdio{
			class util{
			public:
				static int sizeofstr(String str){
					int i;
					for (i = 0; str[i] != 0; i++);
					return i;
				}
				static char charToUpper(char c) {
					return c - 32;
				}
				static char charToLower(char c) {
					return c + 32;
				}
				static char digit_to_char(uint8_t input) {
					switch (input) {
					case 0:
						return '0';
					case 1:
						return '1';
					case 2:
						return '2';
					case 3:
						return '3';
					case 4:
						return '4';
					case 5:
						return '5';
					case 6:
						return '6';
					case 7:
						return '7';
					case 8:
						return '8';
					case 9:
						return '9';
					case 10:
						return 'A';
					case 11:
						return 'B';
					case 12:
						return 'C';
					case 13:
						return 'D';
					case 14:
						return 'E';
					case 15:
						return 'F';
					default:
						return '.';
					}
				}
				static char* number_to_string_template(uint32_t input, uint8_t base, uint8_t sign_bit,
					char* prefix)
				{
					uint8_t prefix_len = sizeofstr(prefix);
					uint8_t negative = sign_bit && ((int32_t)input < 0);

					//ignore the sign bit and invert all bits
					if (sign_bit) {
						if ((int32_t)input < 0) {
							input = ~input;
							input += 1;
						}
						input &= 0x7FFFFFFF;

					}

					uint8_t num_len = 1;
					uint32_t temp = input;
					while ((temp /= base) > 0) {
						num_len++;
					}

					uint8_t full_length = num_len + prefix_len;
					if (negative) {
						full_length++;
					}

					char* str = (char*)CommanderKernael::core::memory::allocateMemory(full_length + 1);
					str[full_length] = 0;

					uint32_t targetmod = 1;
					for (uint16_t i = 0; i < (num_len - 1); i++) {
						targetmod *= base;
					}

					uint16_t start = (negative ? 1 : 0) + prefix_len;
					for (uint8_t i = start; i < num_len + start; i++) {
						str[i] = digit_to_char((input - (input % targetmod)) / targetmod);
						input = input % targetmod;
						targetmod /= base;
					}

					uint8_t prefix_start = 0;
					if (negative) {
						str[0] = '-';
						prefix_start = 1;
					}

					for (uint8_t i = prefix_start; i < prefix_len + prefix_start; i++) {
						str[i] = prefix[i - prefix_start];
					}

					return str;
				}
				static char* uint_to_string(uint32_t input) {
					return number_to_string_template(input, 10, 0, "");
				}
			};
			enum ConsoleColor: char {
				Black,
				Blue,
				Green,
				Cyan,
				Red,
				Magenta,
				Brown,
				LightGray,
				DarkGray,
				LightBlue,
				LightGreen,
				LightCyan,
				LightRed,
				Pink,
				Yellow,
				White
			};
			class string {
			private:
				int lastIndexOf = 0;
			public:
				String _iterator;
				char end_char;
				int lenght() {
					return util::sizeofstr(_iterator);
				}
				string() {
					_iterator = NIL;
				}
				string(String text) {
					_iterator = text;
					end_char = _iterator[util::sizeofstr(_iterator)];
				}
				string(String text, int size) {
					_iterator = (char*)(CommanderKernael::core::memory::allocateMemory(size));
					for (int i = 0; i <= size + 7; i++) {
						_iterator[i] = '\0';
					}
					for (int i = 0; i <= size; i++) {
						_iterator[i] = text[i];
					}
					end_char = _iterator[size];
				}
				int numberOf(char character) {
					int num = 0;
					for (int i = 0; i <= util::sizeofstr(_iterator); i++) if (_iterator[i] == character) num++;
					return num;
				}
				int indexOf(char character) {
					int num = -1;
					for (int i = 0; i <= util::sizeofstr(_iterator); i++) {
						num++;
						if (_iterator[i] == character) break;
					}
					return num;
				}
				int* indexesOf(char character, int* len) {
					int* ind = (int*)CommanderKernael::core::memory::allocateMemory(sizeof(int) * numberOf(character));
					int num = -1;
					int indx = 0;
					for (int i = 0; i <= util::sizeofstr(_iterator); i++) {
						num++;
						if (_iterator[i] == character) {
							ind[indx] = num;
							indx++;
						}
					}
					*len = indx;
					return ind;
				}
				void remove(int startIndex) {
					_iterator[startIndex] = '\0';
				}
				void remove(int startIndex, int count) {
					count -= 1;
					String oldIterator = (String)CommanderKernael::core::memory::allocateMemory(util::sizeofstr(_iterator) + 1, (void*)_iterator);
					CommanderKernael::core::memory::clearMemory(_iterator, util::sizeofstr(_iterator));
					int ind = 0;
					for (int i = 0; i < util::sizeofstr(oldIterator); i++) {
						if (i < startIndex || i > count) {
							_iterator[ind] = oldIterator[i];
							ind++;
						}
					}
				}
				string* split(char separator, int* _len) {
					int len = util::sizeofstr(_iterator);
					int num_delimeters = numberOf(separator);
					//first we count delimeters
					//we won't check the last character because no string would be able to
					//come after it
					char** str_array = (char**)CommanderKernael::core::memory::allocateMemory((sizeof(char*) * (num_delimeters + 1))+2);
					int str_offset = 0;

					int start = 0;
					int end = 0;
					while (end < len) {
						int count = 0;
						while (_iterator[end] != separator && end < len) {
							end++;
							count++;
						}
						//end should hold index of delimeter now
						char* substr = (char*)CommanderKernael::core::memory::allocateMemory(count + 1);
						char* iter = (char*)((uint32_t)_iterator + start);
						int i;
						for (i = 0; i < util::sizeofstr(iter); i++) {
							if (iter[i] == separator){
								substr[i] = '\0';
								break;
							}
							substr[i] = iter[i];
						}
						substr[i] = '\0';
						start = end + 1;
						end++;
						str_array[str_offset] = substr;
						str_offset++;
					}

					//return necessary data now
					string* result = (string*)CommanderKernael::core::memory::allocateMemory(sizeof(string) * str_offset);
					for (int i = 0; i < str_offset; i++) {
						result[i] = string("");
						result[i]._iterator = str_array[i];
						result[i].end_char = '\0';
					}
					*_len = str_offset;
					return result;
				}
				string* split(char separator) {
					int len = util::sizeofstr(_iterator);
					int num_delimeters = numberOf(separator);
					//first we count delimeters
					//we won't check the last character because no string would be able to
					//come after it
					char** str_array = (char**)CommanderKernael::core::memory::allocateMemory((sizeof(char*) * (num_delimeters + 1))+2);
					int str_offset = 0;

					int start = 0;
					int end = 0;
					while (end < len) {
						int count = 0;
						while (_iterator[end] != separator && end < len) {
							end++;
							count++;
						}
						//end should hold index of delimeter now
						char* substr = (char*)CommanderKernael::core::memory::allocateMemory(count + 1);
						char* iter = (char*)((uint32_t)_iterator + start);
						int i;
						for (i = 0; i < util::sizeofstr(iter); i++) {
							if (iter[i] == separator){
								substr[i] = '\0';
								break;
							}
							substr[i] = iter[i];
						}
						substr[i] = '\0';
						start = end + 1;
						end++;
						str_array[str_offset] = substr;
						str_offset++;
					}

					//return necessary data now
					string* result = (string*)CommanderKernael::core::memory::allocateMemory(sizeof(string) * str_offset);
					for (int i = 0; i < str_offset; i++) {
						result[i] = string("");
						result[i]._iterator = str_array[i];
						result[i].end_char = '\0';
					}
					return result;
				}
				bool compare(string string_to_compare) {
					bool ret = true;
					for (int i = 0; ; i++) {
						if (string_to_compare._iterator[i] - _iterator[i] != 0) {
							ret = false;
							break;
						}
						if (string_to_compare._iterator[i] == string_to_compare.end_char && _iterator[i] == end_char) break;
					}
					return ret;
				}
				bool compare(string string_to_compare, char* none, char* noneo, int* ind) {
					bool ret = true;
					for (int i = 0; ; i++) {
						if (string_to_compare._iterator[i] - _iterator[i] != 0) {
							*none = string_to_compare._iterator[i];
							*noneo = _iterator[i];
							*ind = i;
							ret = false;
							break;
						}
						if (string_to_compare._iterator[i] == string_to_compare.end_char && _iterator[i] == end_char) break;
					}
					return ret;
				}
				void destroy() {
					CommanderKernael::core::memory::releaseMemory((void*)_iterator);
				}
				virtual operator int();
				void operator=(int num) {
					if (num == 0) destroy();
				}
				void operator+=(String content) {
					int itsize = util::sizeofstr(_iterator);
					int contsize = util::sizeofstr(content);
					String temp = (String)CommanderKernael::core::memory::allocateMemory(itsize + contsize + 1);
					CommanderKernael::utilities::memory::memcpy((CommanderKernael::utilities::memory::content_t)_iterator, (CommanderKernael::utilities::memory::content_t)temp, itsize);
					CommanderKernael::utilities::memory::memcpy((CommanderKernael::utilities::memory::content_t)content, (CommanderKernael::utilities::memory::content_t)((uint32_t)temp + itsize), contsize);
					temp[itsize + contsize + 1] = '\0';
					CommanderKernael::core::memory::releaseMemory((void*)_iterator, itsize);
					_iterator = temp;
				}
				void operator+=(string content) {
					*this += content._iterator;
				}
				void operator+=(char character) {
					int itsize = util::sizeofstr(_iterator);
					String temp = (String)CommanderKernael::core::memory::allocateMemory(itsize + 2, true);
					CommanderKernael::utilities::memory::memcpy((CommanderKernael::utilities::memory::content_t)_iterator, (CommanderKernael::utilities::memory::content_t)temp, itsize);
					CommanderKernael::utilities::memory::memcpy((CommanderKernael::utilities::memory::content_t)&character, (CommanderKernael::utilities::memory::content_t)((uint32_t)temp + itsize), 1);
					CommanderKernael::core::memory::releaseMemory((void*)_iterator, itsize);
					_iterator = temp;
				}
				void operator+=(int number) {
					*this += intToASCII(number);
				}
				string operator+(string content) {
					string* result = new string(_iterator);
					*result += content;
					return *result;
				}
				string operator+(String content) {
					string* result = new string(_iterator);
					*result += content;
					return *result;
				}
				string operator+(int number) {
					string* result = new string(_iterator);
					*result += number;
					return *result;
				}
				bool operator==(string string_to_compare) {
					bool ret = true;
					for (int i = 0; ; i++) {
						if (string_to_compare._iterator[i] - _iterator[i] != 0) {
							ret = false;
							break;
						}
						if (string_to_compare._iterator[i] == string_to_compare.end_char && _iterator[i] == end_char) break;
					}
					return ret;
				}
				void operator=(string copyFrom) {
					copyFrom.copy(this);
				}
				bool operator!=(string string_to_compare) {
					return !(string_to_compare == *this);
				}
				void reverse() {
					string old = *this;
					old._iterator = (String)CommanderKernael::core::memory::allocateMemory(util::sizeofstr(_iterator), (void*)_iterator);
					int s = util::sizeofstr(_iterator) - 1;
					CommanderKernael::core::memory::clearMemory((void*)_iterator, util::sizeofstr(_iterator));
					int i;
					for (i = 0; s >= 0; s--, i++)
						_iterator[i] = old._iterator[s];
					_iterator[i] = '\0';
				}
				void copy(string* destination) {
					CommanderKernael::core::memory::releaseMemory((void*)destination->_iterator);
					destination->_iterator = (String)(CommanderKernael::core::memory::allocateMemory(util::sizeofstr(_iterator) + 1, (void*)_iterator));
				}
				bool isEmpty() {
					return util::sizeofstr(_iterator) == 0;
				}
			};
			class Console{
			private:
				uint32_t ownership_code;
			public:
				Console(){
					ownership += 0x01;
					this->ownership_code = ownership;
				}
				~Console(){}
				virtual void disableCursor() {
					disable_cursor();
				}
				virtual void setForegroundColor(ConsoleColor color) {
					if (this->ownership_code == ownership)
						changeFColor((char)color);
				}
				virtual void setBackgroundColor(ConsoleColor color) {
					if (this->ownership_code == ownership)
						changeBColor((char)color);
				}
				virtual void resetColors() {
					if (this->ownership_code == ownership) {
						this->setForegroundColor(ConsoleColor::White);
						this->setBackgroundColor(ConsoleColor::Black);
					}
				}
				virtual ConsoleColor getForegroundColor() {
					return (ConsoleColor)(color & 0xF);
				}
				virtual ConsoleColor getBackgroundColor() {
					return (ConsoleColor)((color & 0xF0) >> 4);
				}
				virtual void clear(){
					if (this->ownership_code == ownership)
						clear_screen();
				}
				virtual void printf(String text){
					if (this->ownership_code == ownership)
						kprint(text);
				}
				virtual void printf(char character){
                    String text = "\0";
                    text[0] = character;
					if (this->ownership_code == ownership)
						kprint(text);
				}
				virtual void printf(bool boolean){
                    if (this->ownership_code == ownership){
                        if (boolean) kprint("true");
                        else kprint("false");
                    }
				}
				virtual void printf(int number){
                    if (this->ownership_code == ownership){
						printf(intToASCII(number));
                    }
				}
				virtual void printf(string text) {
					printf(text._iterator);
				}
				virtual void println(){
					if (this->ownership_code == ownership){
						kprint("\n");
					}
				}
				virtual void println(String text){
					if (this->ownership_code == ownership){
						kprint(text);
						kprint("\n");
					}
				}
				virtual void println(char character){
                    String text = "\0";
                    text[0] = character;
					if (this->ownership_code == ownership){
						kprint(text);
                        kprint("\n");
                    }
				}
				virtual void println(bool boolean){
                    if (this->ownership_code == ownership){
                        if (boolean) kprint("true");
                        else kprint("false");
                        kprint("\n");
                    }
				}
				virtual void println(int number){
                    if (this->ownership_code == ownership){
						println(intToASCII(number));
                    }
				}
				virtual void println(string text) {
					println(text._iterator);
				}
				virtual void printf_at(String text, int x, int y){
					if (this->ownership_code == ownership)
						kprint_at(text, x, y);
				}
				virtual bool hasOwnership(){
					if (this->ownership_code == ownership)
						return true;
					else
						return false;
				}
				virtual void takeOwnership(){
					ownership += 0x01;
					this->ownership_code = ownership;
				}
				virtual int cursor_x(){
					return get_offset_col(get_cursor_offset());
				}
				virtual int cursor_y(){
					return get_offset_row(get_cursor_offset());
				}
				virtual void set_cursor_x(int x){
					if (this->ownership_code != ownership)return;
					set_cursor_offset(get_offset(x, cursor_y()));
				}
				virtual void set_cursor_y(int y){
					if (this->ownership_code != ownership)return;
					set_cursor_offset(get_offset(cursor_x(), y));
				}
				virtual void set_cursor_pos(int x, int y){
					if (this->ownership_code != ownership)return;
					set_cursor_offset(get_offset(x, y));
				}
			};
		}
	}
}

CommanderKernael::Drivers::stdio::string operator+(String content1, CommanderKernael::Drivers::stdio::string content2) {
	CommanderKernael::Drivers::stdio::string result = CommanderKernael::Drivers::stdio::string(content1);
	result += content2;
	return result;
}

#endif //__COMMANDERKERNAEL_DRIVERS_STD_STDIO_HPP
