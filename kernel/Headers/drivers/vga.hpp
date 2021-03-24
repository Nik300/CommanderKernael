//Copyright Nicola Leone Ciardi (C)2020
//This file has been licensed by GNU-GPL
//All rights reserved. (C)2020
//Only contributors are authorized to modify/redistribute
//these files

//See CONTRIBUTORS.TXT for more infos

#ifndef __COMMANDERKERNAEL_DRIVERS_VGA_HPP
#define __COMMANDERKERNAEL_DRIVERS_VGA_HPP

#include <Headers/environment.hpp>
#include <Headers/drivers/driver.hpp>
#include <Headers/stdio.hpp>
#include <Headers/types.hpp>
#include <Headers/core.hpp>
#include <Headers/ports.hpp>
#include <Headers/util.hpp>
#include <Headers/Graphics/VideoAbstractionLayer.hpp>

using namespace CommanderKernael::Drivers;
using namespace CommanderKernael::Drivers::stdio;
using namespace CommanderKernael::utilities::ASCII_UTIL;
using namespace CommanderKernael::utilities::memory;
using namespace assembly::ports;
using namespace assembly::types;
using namespace CommanderKernael::core;

namespace CommanderKernael{
	namespace Drivers{
		class VideoGraphicsArray: public VAL{
        protected:
            port8bit miscPort = port8bit(0x3c2);
            port8bit crtcIndexPort = port8bit(0x3d4);
            port8bit crtcDataPort = port8bit(0x3d5);
            port8bit sequencerIndexPort = port8bit(0x3c4);
            port8bit sequencerDataPort = port8bit(0x3c5);
            port8bit graphicsControllerIndexPort = port8bit(0x3ce);
            port8bit graphicsControllerDataPort = port8bit(0x3cf);
            port8bit attributeControllerIndexPort = port8bit(0x3c0);
            port8bit attributeControllerreadPort = port8bit(0x3c1);
            port8bit attributeControllerwritePort = port8bit(0x3c0);
            port8bit attributeControllerResetPort = port8bit(0x3da);

            uint16_t height, width;
            uint8_t depth, PixelStride;

            int32_t pitch;

            void writeRegisters(uint8_t* registers);
            uint8_t* GetFrameBufferSegment();

        public:
            CommanderKernael::core::memory::MemoryBlock<uint8_t> buffer;

            VideoGraphicsArray(): VAL(){}
            ~VideoGraphicsArray(){}

            bool SupportsMode(uint16_t width, uint16_t height, uint8_t colordepth);
            virtual bool SetMode(uint16_t width, uint16_t height, uint8_t colordepth);
            virtual void SetPixel(int x, int y, uint32_t color);
            virtual void Render();

		};
	}
}

void VideoGraphicsArray::writeRegisters(uint8_t* registers)
{
    //  misc
    miscPort.write(*(registers++));
    
    // sequencer
    for(uint8_t i = 0; i < 5; i++)
    {
        sequencerIndexPort.write(i);
        sequencerDataPort.write(*(registers++));
    }
    
    // cathode ray tube controller
    crtcIndexPort.write(0x03);
    crtcDataPort.write(crtcDataPort.read() | 0x80);
    crtcIndexPort.write(0x11);
    crtcDataPort.write(crtcDataPort.read() & ~0x80);
    
    registers[0x03] = registers[0x03] | 0x80;
    registers[0x11] = registers[0x11] & ~0x80;
    
    for(uint8_t i = 0; i < 25; i++)
    {
        crtcIndexPort.write(i);
        crtcDataPort.write(*(registers++));
    }
    
    // graphics controller
    for(uint8_t i = 0; i < 9; i++)
    {
        graphicsControllerIndexPort.write(i);
        graphicsControllerDataPort.write(*(registers++));
    }
    
    // attribute controller
    for(uint8_t i = 0; i < 21; i++)
    {
        attributeControllerResetPort.read();
        attributeControllerIndexPort.write(i);
        attributeControllerwritePort.write(*(registers++));
    }
    
    attributeControllerResetPort.read();
    attributeControllerIndexPort.write(0x20);
}

void VideoGraphicsArray::Render(){
    memcpy(buffer.ToList<char>(), (char*)GetFrameBufferSegment(), this->height * this->pitch);
}

bool VideoGraphicsArray::SupportsMode(uint16_t width, uint16_t height, uint8_t colordepth)
{
    return width == 320 && height == 200 && colordepth == 8;
}

bool VideoGraphicsArray::SetMode(uint16_t width, uint16_t height, uint8_t colordepth)
{
    if(!SupportsMode(width, height, colordepth))
        return false;
    
    unsigned char g_320x200x256[] =
    {
        /* MISC */
            0x63,
        /* SEQ */
            0x03, 0x01, 0x0F, 0x00, 0x0E,
        /* CRTC */
            0x5F, 0x4F, 0x50, 0x82, 0x54, 0x80, 0xBF, 0x1F,
            0x00, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x9C, 0x0E, 0x8F, 0x28, 0x40, 0x96, 0xB9, 0xA3,
            0xFF,
        /* GC */
            0x00, 0x00, 0x00, 0x00, 0x00, 0x40, 0x05, 0x0F,
            0xFF,
        /* AC */
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
            0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
            0x41, 0x00, 0x0F, 0x00, 0x00
    };

    this->height = height;
    this->width = width;

    this->PixelStride = (colordepth | 7) >> 3;

    this->pitch = width * PixelStride;

    this->buffer = CommanderKernael::core::memory::MemoryBlock<uint8_t>(this->height * this->pitch);

    this->depth = colordepth;
    
    writeRegisters(g_320x200x256);
    return true;
}

uint8_t* VideoGraphicsArray::GetFrameBufferSegment()
{
    graphicsControllerIndexPort.write(0x06);
    uint8_t segmentNumber = graphicsControllerDataPort.read() & (3<<2);
    switch(segmentNumber)
    {
        default:
        case 0<<2: return (uint8_t*)0x00000;
        case 1<<2: return (uint8_t*)0xA0000;
        case 2<<2: return (uint8_t*)0xB0000;
        case 3<<2: return (uint8_t*)0xB8000;
    }
}

void VideoGraphicsArray::SetPixel(int x, int y, uint32_t color)
{
    if (color > 255) return;
    if(x < 0 || width <= x
    || y < 0 || height <= y)
        return;
        
    buffer.Write(width * y + x, (uint8_t)color);
}

#endif //__COMMANDERKERNAEL_DRIVERS_VGA_HPP