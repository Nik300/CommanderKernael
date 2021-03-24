#ifndef __COMMANDERKERNAEL_DRIVERS_VAL_HPP
#define __COMMANDERKERNAEL_DRIVERS_VAL_HPP

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
	namespace Drivers {
		class VAL {
        public:
            uint64_t width;
            uint64_t height;
			virtual bool SetMode(uint16_t width, uint16_t height, uint8_t colordepth) {}
			virtual void SetPixel(int x, int y, uint32_t color) {}
            virtual void Render() {}
            virtual void Disable() {}
			virtual void RenderMouse(int16_t x, int16_t y, uint32_t mColor = 0x0f, uint32_t sColor = 0x01) {
                SetPixel(x + 1, y + 1, mColor);
                SetPixel(x + 1, y + 2, mColor);
                SetPixel(x + 2, y + 3, mColor);
                SetPixel(x + 2, y + 4, mColor);
                SetPixel(x + 3, y + 5, mColor);
                SetPixel(x + 3, y + 6, mColor);
                SetPixel(x + 4, y + 7, mColor);
                SetPixel(x + 4, y + 8, mColor);
                SetPixel(x + 5, y + 9, mColor);
                SetPixel(x + 5, y + 10, mColor);
                SetPixel(x + 6, y + 11, mColor);
                SetPixel(x + 6, y + 12, mColor);

                SetPixel(x + 2, y + 1, mColor);
                SetPixel(x + 3, y + 2, mColor);
                SetPixel(x + 4, y + 2, mColor);
                SetPixel(x + 5, y + 3, mColor);
                SetPixel(x + 6, y + 3, mColor);
                SetPixel(x + 7, y + 4, mColor);
                SetPixel(x + 8, y + 4, mColor);
                SetPixel(x + 9, y + 5, mColor);
                SetPixel(x + 10, y + 5, mColor);
                SetPixel(x + 11, y + 6, mColor);
                SetPixel(x + 12, y + 6, mColor);

                SetPixel(x + 6, y + 12, mColor);
                SetPixel(x + 7, y + 11, mColor);
                SetPixel(x + 8, y + 10, mColor);
                SetPixel(x + 9, y + 9, mColor);
                SetPixel(x + 10, y + 8, mColor);
                SetPixel(x + 11, y + 7, mColor);
                SetPixel(x + 12, y + 6, mColor);

                SetPixel(x, y, sColor);
                SetPixel(x, y + 1, sColor);
                SetPixel(x, y + 2, sColor);
                SetPixel(x + 1, y + 3, sColor);
                SetPixel(x + 1, y + 4, sColor);
                SetPixel(x + 2, y + 5, sColor);
                SetPixel(x + 2, y + 6, sColor);
                SetPixel(x + 3, y + 7, sColor);
                SetPixel(x + 3, y + 8, sColor);
                SetPixel(x + 4, y + 9, sColor);
                SetPixel(x + 4, y + 10, sColor);
                SetPixel(x + 5, y + 11, sColor);
                SetPixel(x + 5, y + 12, sColor);
                SetPixel(x + 6, y + 13, sColor);

                SetPixel(x + 1, y, sColor);
                SetPixel(x + 2, y, sColor);
                SetPixel(x + 3, y + 1, sColor);
                SetPixel(x + 4, y + 1, sColor);
                SetPixel(x + 5, y + 2, sColor);
                SetPixel(x + 6, y + 2, sColor);
                SetPixel(x + 7, y + 3, sColor);
                SetPixel(x + 8, y + 3, sColor);
                SetPixel(x + 9, y + 4, sColor);
                SetPixel(x + 10, y + 4, sColor);
                SetPixel(x + 11, y + 5, sColor);
                SetPixel(x + 12, y + 5, sColor);
                SetPixel(x + 13, y + 6, sColor);

                SetPixel(x + 7, y + 13, sColor);
                SetPixel(x + 8, y + 12, sColor);
                SetPixel(x + 9, y + 11, sColor);
                SetPixel(x + 10, y + 10, sColor);
                SetPixel(x + 11, y + 9, sColor);
                SetPixel(x + 12, y + 8, sColor);
                SetPixel(x + 13, y + 7, sColor);
			}
		};
	}
}

#endif