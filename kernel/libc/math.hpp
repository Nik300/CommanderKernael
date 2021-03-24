#ifndef __LIBC_SYSTEM_MATH_HPP
#define __LIBC_SYSTEM_MATH_HPP

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

namespace System {
    namespace math {
        int pow(int number, int power){
            if (power == 0) return 1;
            if (power == 1) return number;
            int result = number;
            for (int i = 1; i < power; i++){
                result *= number;
            }
            return result;
        }
		int sizeOfInteger(int number){
			int result = 0;
			while (!(number >= 0 && number <= 9))
			{
				number /= 10;
				result++;
			}
			return result+1;
		}
    }
}

#endif //__LIBC_SYSTEM_MATH_HPP