#ifndef __UTIL_HPP
#define __UTIL_HPP

void memory_copy(char *source, char *dest, int nbytes);
void memset(char* destination, char content, int lenght);
void memcpy(char* destination, char* source, int lenght);
void int_to_ascii(int n, char str[]);

#endif // __UTIL_HPP