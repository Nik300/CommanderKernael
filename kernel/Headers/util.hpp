#ifndef __COMMANDERKERNAEL_UTILITIES_UTILITY_HPP
#define __COMMANDERKERNAEL_UTILITIES_UTILITY_HPP

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
                for (int i = 0; i <= lenght + 1; i++) *(destination + i) = content;
			}
			void memcpy(content_int_t source, content_int_t destination, int lenght){
				for (int i = 0; i <= lenght; i++) if (*(destination + i) != *(source + i)) *(destination + i) = *(source + i);
			}
			void memset(uint32_t content, content_int_t destination, int lenght){
                for (int i = 0; i <= lenght + 1; i++) *(destination + i) = content;
			}
		}
		namespace ASCII_UTIL{
			char* intToASCII(int number){
                char *num;
                char *res;
				int_to_ascii(number, num);
				int s = 0;
				for(int i = sizeof(num) - 1; i >= 0; i--){
                    res[s] = num[i];
                    s++;
                }
                res[sizeof(num)] = '\0';
				return res;
			}
		}
	}
}

#endif //__COMMANDERKERNAEL_UTILITIES_UTILITY_HPP
