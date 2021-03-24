#ifndef __COMMANDERKERNAEL_UTILITIES_VIDEOUTILS_HPP
#define __COMMANDERKERNAEL_UTILITIES_VIDEOUTILS_HPP

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

namespace CommanderKernael {
	namespace utilities {
		namespace videoutils {
			uint32_t GetRGB(uint8_t r, uint8_t g, uint8_t b)
			{
				uint32_t c;
				c = r;
				c <<= 8;
				c |= g;
				c <<= 8;
				c |= b;
				return c;
			}
			uint32_t Get32Color(ConsoleColor color){
				switch (color){
					case ConsoleColor::Black:
						return GetRGB(0, 0, 0);
					case ConsoleColor::Blue:
						return GetRGB(0, 0, 128);
					case ConsoleColor::Green:
						return GetRGB(0,128,0);
					case ConsoleColor::Cyan:
						return GetRGB(0, 128, 128);
					case ConsoleColor::Red:
						return GetRGB(128, 0, 0);
					case ConsoleColor::Magenta:
						return GetRGB(255, 0, 255);
					case ConsoleColor::Brown:
						return GetRGB(165, 42, 42);
					case ConsoleColor::LightGray:
						return GetRGB(211, 211, 211);
					case ConsoleColor::DarkGray:
						return GetRGB(169, 169, 169);
					case ConsoleColor::LightBlue:
						return GetRGB(0, 0, 255);
					case ConsoleColor::LightGreen:
						return GetRGB(0, 255, 0);
					case ConsoleColor::LightCyan:
						return GetRGB(0, 255, 255);
					case ConsoleColor::LightRed:
						return GetRGB(255, 0, 0);
					case ConsoleColor::Pink:
						return GetRGB(255, 192, 203);
					case ConsoleColor::Yellow:
						return GetRGB(255, 255, 0);
					case ConsoleColor::White:
						return GetRGB(255, 255, 255);
					default:
						break;
				}
				return 0;
			}
		}
	}
}

#endif //__COMMANDERKERNAEL_UTILITIES_VIDEOUTILS_HPP