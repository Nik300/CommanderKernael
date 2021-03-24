#ifndef __LIBC_SYSTEM_OPERATORS_HPP
#define __LIBC_SYSTEM_OPERATORS_HPP

#include <Headers/environment.hpp>
#include <Headers/drivers/driver.hpp>
#include <Headers/stdio.hpp>
#include <Headers/types.hpp>
#include <Headers/core.hpp>
#include <Headers/ports.hpp>
#include <Headers/util.hpp>
#include <libc/math.hpp>

using namespace CommanderKernael::Drivers;
using namespace CommanderKernael::Drivers::stdio;
using namespace CommanderKernael::utilities::ASCII_UTIL;
using namespace CommanderKernael::utilities::memory;
using namespace assembly::ports;
using namespace assembly::types;
using namespace CommanderKernael::core;
using namespace System::math;

namespace CommanderKernael {
    namespace Drivers {
        namespace stdio{
            string::operator int(){
                int result = 0;
                result = 0;
                if (_iterator[0] == '-'){
                    for (int i = 1; i < lenght(); i++){
                        result += (numFromChar(_iterator[i]) * pow(10, (lenght()-2) - (i-1)));
                    }
                    return -(result);
                }
                for (int i = 0; i < lenght(); i++){
                    result += (numFromChar(_iterator[i]) * pow(10, (lenght()-1) - i));
                }
                return result;
            }
        }
    }
}
namespace System{
    namespace operators{
        int StringToInt(string str){
                int result;
                result = 0;
                if (str._iterator[0] == '-'){
                    for (int i = 1; i < str.lenght(); i++){
                        result += (numFromChar(str._iterator[i]) * pow(10, (str.lenght()-2) - (i-1)));
                    }
                    return -(result);
                }
                for (int i = 0; i < str.lenght(); i++){
                    result += (numFromChar(str._iterator[i]) * pow(10, (str.lenght()-1) - i));
                }
                return result;
        }
    }
}

#endif //__LIBC_SYSTEM_OPERATORS_HPP