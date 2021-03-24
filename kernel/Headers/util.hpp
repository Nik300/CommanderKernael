#ifndef __COMMANDERKERNAEL_UTILITIES_UTILITY_HPP
#define __COMMANDERKERNAEL_UTILITIES_UTILITY_HPP

#include <Headers/ports.hpp>

using namespace assembly::ports;

extern "C" void memory_copy(char*, char*, int);
extern "C" void int_to_ascii(int, char[]);

namespace CommanderKernael{
	namespace utilities{
		namespace memory{
			typedef char* content_t;
			typedef unsigned short* content_short_t;
			typedef unsigned int* content_int_t;
			void memcpy(content_t source, content_t destination, int lenght){
				for (int i = 0; i <= lenght; i++) if (*(destination + i) != *(source + i)) *(destination + i) = *(source + i);
			}
			void memset(uint8_t content, content_t destination, int lenght){
                for (int i = 0; i <= lenght + 1; i++) *(destination + i) = content;
			}
			void memcpy(content_short_t source, content_short_t destination, int lenght){
				for (int i = 0; i <= lenght; i++) if (*(destination + i) != *(source + i)) *(destination + i) = *(source + i);
			}
			void memset(uint16_t content, content_short_t destination, int lenght){
                for (int i = 0; i <= lenght + 1; i++) if (*(destination + i) != content) *(destination + i) = content;
			}
			void memcpy(content_int_t source, content_int_t destination, int lenght){
				for (int i = 0; i <= lenght; i++) if (*(destination + i) != *(source + i)) *(destination + i) = *(source + i);
			}
			void memset(uint32_t content, content_int_t destination, int lenght){
                for (int i = 0; i <= lenght + 1; i++) if (*(destination + i) != content) *(destination + i) = content;
			}
		}
		namespace ASCII_UTIL {
			int intSize(int number){
				int result = 0;
				while (!(number >= 0 && number <= 9))
				{
					number /= 10;
					result++;
				}
				return result+1;
			}
			char* intToASCII(int number) {
				char* num = (char*)core::memory::allocateMemory(intSize(number));
				int_to_ascii(number, num);
				return num;
			}
			uint8_t numFromChar(char n){
				switch(n){
					case '0':
						return 0;
					case '1':
						return 1;
					case '2':
						return 2;
					case '3':
						return 3;
					case '4':
						return 4;
					case '5':
						return 5;
					case '6':
						return 6;
					case '7':
						return 7;
					case '8':
						return 8;
					case '9':
						return 9;
				}
			}
		}
	}
}

#endif //__COMMANDERKERNAEL_UTILITIES_UTILITY_HPP
