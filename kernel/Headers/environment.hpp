//Copyright Nicola Leone Ciardi (C)2020
//This file has been licensed by GNU-GPL
//All rights reserved. (C)2020
//Only contributors are authorized to modify/redistribute
//these files

//See CONTRIBUTORS.TXT for more infos

#ifndef __COMMANDERKERNAEL_ENVIRONMENT_CORE_HPP
#define __COMMANDERKERNAEL_ENVIRONMENT_CORE_HPP

#include <Headers/drivers/driver.hpp>
#include <Headers/drivers/keyboard.hpp>
#include <Headers/stdio.hpp>
#include <Headers/types.hpp>
#include <Headers/core.hpp>
#include <Headers/ports.hpp>
using namespace CommanderKernael;
using namespace keyboardSchemes;
using namespace CommanderKernael::Drivers;
using namespace CommanderKernael::utilities;
using namespace CommanderKernael::Drivers::stdio;

namespace CommanderKernael{
    namespace Environment{
        namespace SystemUser{
            namespace selection{
                static CommanderKernael::keyboardSchemes::keyboardScheme* preferredKeyboardScheme;
                static String name = "System authority";
                static String username = "systemauthorityservice";
            }
        }
    }
}

#endif //__COMMANDERKERNAEL_ENVIRONMENT_CORE_HPP
