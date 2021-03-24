#ifndef __LIBC_SYSTEM_RANDOM_HPP
#define __LIBC_SYSTEM_RANDOM_HPP

#include <Headers/types.hpp>
#include <Headers/core.hpp>
#include <libc/collections.hpp>

using namespace assembly::types;

namespace System{
    namespace Random{
        uint64_t GlobalSeed = 0;
        class Rand{
        private:
            int seed = 1;
            int _seed = 0;
        public:
            uint32_t RandInt(int max){
                if (!_seed)
                    seed = GlobalSeed + seed * 1103515245 +12345;
                else
                    seed = _seed + seed * 1103515245 +12345;
                GlobalSeed = seed;
                return (uint32_t)(seed / 65536) % (max+1);
            }
            uint32_t RandInt(int min, int max){
                seed = GlobalSeed + seed * 1103515245 +12345;
                GlobalSeed = seed;
                uint32_t res = (uint32_t)(seed / 65536) % (max+1);
                if (res < min) return RandInt(min, max);
                return (uint32_t)(seed / 65536) % (max+1);
            }
            string RandString(int lenght){
                static string letters = "AaBbCcDdEeFfGgHhIiLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz1234567890!_<>";
                char* result = (char*)CommanderKernael::core::memory::allocateMemory(lenght);
                for (int i = 0; i < lenght; i++)
                    result[i] = letters._iterator[RandInt(0, letters.lenght() - 1)];
                return result;
            }
            void SetSeed(int seed){
                this->_seed = seed;
            }
        };
    }
}

#endif //__LIBC_SYSTEM_RANDOM_HPP