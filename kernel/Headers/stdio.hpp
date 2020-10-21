#ifndef __COMMANDERKERNAEL_DRIVERS_STD_STDIO_HPP
#define __COMMANDERKERNAEL_DRIVERS_STD_STDIO_HPP

#include "Headers/types.hpp"

using namespace assembly::types;

extern "C" void clear_screen();
extern "C" void kprint_at(char*, int, int);
extern "C" void kprint(char*);
extern "C" int get_cursor_offset();
extern "C" void set_cursor_offset(int offset);
extern "C" int get_offset(int col, int row);
extern "C" int get_offset_row(int offset);
extern "C" int get_offset_col(int offset);

uint32_t ownership = 0x0000080;

namespace CommanderKernael{
	namespace Drivers{
		namespace stdio{
			class Console{
			private:
				uint32_t ownership_code;
			public:
				Console(){
					ownership += 0x01;
					this->ownership_code = ownership;
				}
				~Console(){}
				void clear(){
					if (this->ownership_code == ownership)
						clear_screen();
				}
				void printf(String text){
					if (this->ownership_code == ownership)
						kprint(text);
				}
				void printf(char character){
                    String text = "\0";
                    text[0] = character;
					if (this->ownership_code == ownership)
						kprint(text);
				}
				void printf(bool boolean){
                    if (this->ownership_code == ownership){
                        if (boolean) kprint("true");
                        else kprint("false");
                    }
				}
				void printf(uint8_t key){
                    if (this->ownership_code == ownership){
                        char* foo = "00";
                        char* hex = "0123456789ABCDEF";
                        foo[0] = hex[(key >> 4) & 0xF];
                        foo[1] = hex[key & 0xF];
                        printf(foo);
                    }
				}
				void println(){
					if (this->ownership_code == ownership){
						kprint("\n");
					}
				}
				void println(String text){
					if (this->ownership_code == ownership){
						kprint(text);
						kprint("\n");
					}
				}
				void println(char character){
                    String text = "\0";
                    text[0] = character;
					if (this->ownership_code == ownership){
						kprint(text);
                        kprint("\n");
                    }
				}
				void println(bool boolean){
                    if (this->ownership_code == ownership){
                        if (boolean) kprint("true");
                        else kprint("false");
                        kprint("\n");
                    }
				}
				void println(uint8_t key){
                    if (this->ownership_code == ownership){
                        char* foo = "00";
                        char* hex = "0123456789ABCDEF";
                        foo[0] = hex[(key >> 4) & 0xF];
                        foo[1] = hex[key & 0xF];
                        println(foo);
                    }
				}
				void printf_at(String text, int x, int y){
					if (this->ownership_code == ownership)
						kprint_at(text, x, y);
				}
				bool hasOwnership(){
					if (this->ownership_code == ownership)
						return true;
					else
						return false;
				}
				void takeOwnership(){
					ownership += 0x01;
					this->ownership_code = ownership;
				}
				int cursor_x(){
					return get_offset_col(get_cursor_offset());
				}
				int cursor_y(){
					return get_offset_row(get_cursor_offset());
				}
				void set_cursor_x(int x){
					if (this->ownership_code != ownership)return;
					set_cursor_offset(get_offset(x, cursor_y()));
				}
				void set_cursor_y(int y){
					if (this->ownership_code != ownership)return;
					set_cursor_offset(get_offset(cursor_x(), y));
				}
				void set_cursor_pos(int x, int y){
					if (this->ownership_code != ownership)return;
					set_cursor_offset(get_offset(x, y));
				}
			};
            class string{
            private:
                Console _console = Console();
            public:
                String _iterator = 0x0000;
                char end_char;
                string(String text){
                    end_char = text[sizeof(text) + 1];
                    _iterator = text;
                }
                void get_frame_buffer_segment(String buffer_segment){
                    buffer_segment = _iterator;
                }
                bool compare(string string_to_compare){
                    String buffer_seg;
                    string_to_compare.get_frame_buffer_segment(buffer_seg);
                    bool ret = true;
                    for (int i = 0; ; i++){
                        if (buffer_seg[i] - _iterator[i] != 0){
                            ret = false;
                            break;
                        }
						if (i == sizeof(_iterator) && i == sizeof(buffer_seg))
							if (_iterator[sizeof(_iterator)+1] == end_char && buffer_seg[sizeof(buffer_seg)+1] == string_to_compare.end_char) break;
                    }
                    return ret;
                }
            };
		}
	}
}

#endif //__COMMANDERKERNAEL_DRIVERS_STD_STDIO_HPP
