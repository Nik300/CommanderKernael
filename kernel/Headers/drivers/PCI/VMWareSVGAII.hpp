//This is the driver for VMWare SVGA II PCI device

//Written by: Nicola Leone Ciardi

//State: working

#ifndef __COMMANDERKERNAEL_DRIVERS_PCI_VMWareSVGAII_HPP
#define __COMMANDERKERNAEL_DRIVERS_PCI_VMWareSVGAII_HPP

#include <Headers/environment.hpp>
#include <Headers/drivers/driver.hpp>
#include <Headers/stdio.hpp>
#include <Headers/types.hpp>
#include <Headers/core.hpp>
#include <Headers/ports.hpp>
#include <Headers/util.hpp>
#include <Headers/pci.hpp>

using namespace CommanderKernael::Drivers;
using namespace CommanderKernael::Drivers::stdio;
using namespace CommanderKernael::utilities::ASCII_UTIL;
using namespace CommanderKernael::utilities::memory;
using namespace assembly::ports;
using namespace assembly::types;
using namespace CommanderKernael::core;
using namespace CommanderKernael::core::HAL::supports::PCI;

namespace CommanderKernael{
	namespace Drivers{
		namespace PCI{
			namespace VMWareDrivers{
				enum Register : uint16_t{
					IDr = 0,
					Enable = 1,
					Width = 2,
					Height = 3,
					MaxWidth = 4,
					MaxHeight = 5,
					Depth = 6,
					BitsPerPixel = 7,
					PseudoColor = 8,
					RedMask = 9,
					GreenMask = 10,
					BlueMask = 11,
					BytesPerLine = 12,
					FrameBufferStart = 13,
					FrameBufferOffset = 14,
					VRamSize = 15,
					FrameBufferSize = 16,
					Capabilities = 17,
					MemStart = 18,
					MemSize = 19,
					ConfigDone = 20,
					Sync = 21,
					Busy = 22,
					GuestID = 23,
					CursorID = 24,
					CursorX = 25,
					CursorY = 26,
					CursorOn = 27,
					HostBitsPerPixel = 28,
					ScratchSize = 29,
					MemRegs = 30,
					NumDisplays = 31,
					PitchLockReg = 32,
					FifoNumRegisters = 293
				};
				enum ID : uint32_t{
					Magic = 0x900000,
					V0 = Magic << 8,
					V1 = (Magic << 8) | 1,
					V2 = (Magic << 8) | 2,
					Invalid = 0xFFFFFFFF
				};
				enum FIFO : uint32_t{
					Min = 0,
					Max = 4,
					NextCmd = 8,
					Stop = 12
				};
				enum FIFOCommand{
					Update = 1,
					RECT_FILL = 2,
					RECT_COPY = 3,
					DEFINE_BITMAP = 4,
					DEFINE_BITMAP_SCANLINE = 5,
					DEFINE_PIXMAP = 6,
					DEFINE_PIXMAP_SCANLINE = 7,
					RECT_BITMAP_FILL = 8,
					RECT_PIXMAP_FILL = 9,
					RECT_BITMAP_COPY = 10,
					RECT_PIXMAP_COPY = 11,
					FREE_OBJECT = 12,
					RECT_ROP_FILL = 13,
					RECT_ROP_COPY = 14,
					RECT_ROP_BITMAP_FILL = 15,
					RECT_ROP_PIXMAP_FILL = 16,
					RECT_ROP_BITMAP_COPY = 17,
					RECT_ROP_PIXMAP_COPY = 18,
					DEFINE_CURSOR = 19,
					DISPLAY_CURSOR = 20,
					MOVE_CURSOR = 21,
					DEFINE_ALPHA_CURSOR = 22
				};
				enum IOPortOffset : uint8_t{
					Index = 0,
					Value = 1,
					Bios = 2,
					IRQ = 3
				};
				enum Capability{
					None = 0,
					RectFill = 1,
					RectCopy = 2,
					RectPatFill = 4,
					LecacyOffscreen = 8,
					RasterOp = 16,
					Cursor = 32,
					CursorByPass = 64,
					CursorByPass2 = 128,
					EigthBitEmulation = 256,
					AlphaCursor = 512,
					Glyph = 1024,
					GlyphClipping = 0x00000800,
					Offscreen1 = 0x00001000,
					AlphaBlend = 0x00002000,
					ThreeD = 0x00004000,
					ExtendedFifo = 0x00008000,
					MultiMon = 0x00010000,
					PitchLock = 0x00020000,
					IrqMask = 0x00040000,
					DisplayTopology = 0x00080000,
					Gmr = 0x00100000,
					Traces = 0x00200000,
					Gmr2 = 0x00400000,
					ScreenObject2 = 0x00800000
				};

				class VMWareSVGAII{
				private:
                    PCIController controller = PCIController();
					uint32_t* Video_Memory;
					bool enabled = false;
					int32_t* FIFO_Memory;
					PCIDevice device = controller.RequireDevice(0x15AD, 0x0405);
					port32bit IndexPort = port32bit((uint16_t)(device.portBase + IOPortOffset::Index));
					port32bit ValuePort = port32bit((uint16_t)(device.portBase + IOPortOffset::Value));
					port32bit BiosPort = port32bit((uint16_t)(device.portBase + IOPortOffset::Bios));
					port32bit IRQPort = port32bit((uint16_t)(device.portBase + IOPortOffset::IRQ));
					int pitch;
					uint32_t capabilities;
					uint32_t* buffer;
					bool buffered;
				public:
					uint32_t width;
					uint32_t height;
					uint32_t depth;
					uint32_t real_depth;
					VMWareSVGAII(){
						controller.EnableMemory(true, &device);
						WriteRegister(Register::IDr, (uint32_t)ID::V2);

						if (ReadRegister(Register::IDr) != (uint32_t)ID::V2) {
							WriteRegister(Register::IDr, (uint32_t)ID::V2);
							if (ReadRegister(Register::IDr) != (uint32_t)ID::V2) return;
						}

						enabled = true;

						Video_Memory = (uint32_t*)(ReadRegister(Register::FrameBufferStart));

						capabilities = ReadRegister(Register::Capabilities);
					}
					~VMWareSVGAII(){}
					void WriteRegister(Register reg, uint32_t data){
						IndexPort.write((uint32_t)reg);
						ValuePort.write((uint32_t)data);
					}
					uint32_t ReadRegister(Register reg){
						IndexPort.write((uint32_t)reg);
						return ValuePort.read();
					}
					void InitializeFIFO(){
						FIFO_Memory = (int32_t*)(ReadRegister(Register::MemStart));
						FIFO_Memory[(uint32_t)FIFO::Min] = (uint32_t)Register::FifoNumRegisters * sizeof(uint32_t);
						FIFO_Memory[(uint32_t)FIFO::Max] = (uint32_t)ReadRegister(Register::MemSize);
						FIFO_Memory[(uint32_t)FIFO::NextCmd] = FIFO_Memory[(uint32_t)FIFO::Min];
						FIFO_Memory[(uint32_t)FIFO::Stop] = FIFO_Memory[(uint32_t)FIFO::Min];
						WriteRegister(Register::ConfigDone, 1);
					}
					uint32_t Color(uint8_t r, uint8_t g, uint8_t b)
					{
						uint32_t c;
						 c = r;
						 c <<= 8;
						 c |= g;
						 c <<= 8;
						 c |= b;
						 return c;
					}
					void SetMode(uint32_t width, uint32_t height, uint32_t depth = 32, bool buffered = true)
					{
						this->depth = (depth / 8);
						this->width = width;
						this->height = height;
						this->real_depth = depth;
						this->buffered = buffered;
						if (buffered){
							int8_t PixelStride = (depth | 7) >> 3;
							this->pitch = width * PixelStride;
							this->buffer = (uint32_t*)(height*this->pitch);
						}
						WriteRegister(Register::Width, width);
						WriteRegister(Register::Height, height);
						WriteRegister(Register::BitsPerPixel, depth);
						InitializeFIFO();
					}
					void UpdateMode(){
						WriteRegister(Register::Width, width);
						WriteRegister(Register::Height, height);
						WriteRegister(Register::BitsPerPixel, real_depth);
					}
					uint32_t GetFifo(FIFO cmd){
						return FIFO_Memory[(uint32_t)cmd];
					}
					void SetFifo(FIFO cmd, int32_t value){
						FIFO_Memory[(int32_t)cmd] = value;
					}
					void WaitForFIFO(){
						WriteRegister(Register::Sync, 1);
						while (ReadRegister(Register::Busy) != 0);
					}
					void WriteToFIFO(int32_t _value){
						if (((GetFifo(FIFO::NextCmd) == GetFifo(FIFO::Max) - 4) && GetFifo(FIFO::Stop) == GetFifo(FIFO::Min)) ||
						   (GetFifo(FIFO::NextCmd) + 4 == GetFifo(FIFO::Stop)))
							WaitForFIFO(); 

						SetFifo((FIFO)(GetFifo(FIFO::NextCmd)), _value);
						SetFifo(FIFO::NextCmd, GetFifo(FIFO::NextCmd) + 4);

						if (GetFifo(FIFO::NextCmd) == GetFifo(FIFO::Max))
							SetFifo(FIFO::NextCmd, GetFifo(FIFO::Min));
					}
					void Update(int32_t x, int32_t y, int32_t _width, int32_t _height){
						WriteToFIFO(((int32_t)FIFOCommand::Update));
						WriteToFIFO(x);
						WriteToFIFO(y);
						WriteToFIFO(_width);
						WriteToFIFO(_height);
						WaitForFIFO();
					}
					void SetPixel(uint32_t x, uint32_t y, uint32_t color){
						if (!enabled) WriteRegister(Register::Enable, 0);
						if (!buffered){
							if (Video_Memory[((y * width + x))] != color)
								Video_Memory[((y * width + x))] = color;
						}
						else{
							if (buffer[width*y+x] != color)
								buffer[width*y+x] = color;
						}
					}
					void Render(){
						if (buffered) memcpy(buffer, Video_Memory, height*this->pitch);
						this->Update(0, 0, this->width, this->height);
					}
					uint32_t GetPixel(uint32_t x, uint32_t y){
						return Video_Memory[((y * width + x) * depth)];
					}
					void fill(uint32_t x, uint32_t y, uint32_t _width, uint32_t _height, uint32_t color){
						if ((capabilities & (uint32_t)Capability::RectFill) != 0)
						{
							WriteToFIFO((uint32_t)FIFOCommand::RECT_FILL);
							WriteToFIFO(color);
							WriteToFIFO(x);
							WriteToFIFO(y);
							WriteToFIFO(_width);
							WriteToFIFO(_height);
							WaitForFIFO();
						}
						else{
						}
					}
					void Activate(){
						WriteRegister(Register::Enable, 1);
					}
					void Deactivate(){
						Console console = Console();
						console.printf((unsigned char)(((uint32_t)(Video_Memory) & 0xFF0000) >> 16));
						console.printf((unsigned char)(((uint32_t)(Video_Memory) & 0xFF00) >> 8));
						console.printf((unsigned char)(((uint32_t)Video_Memory) & 0xFF));
						WriteRegister(Register::Enable, 0);
					}
					void Hang(){
						if (ReadRegister(Register::IDr) != (uint32_t)ID::V2) return;
						else while(true);
					}
				};
			}
		}
	}
}

#endif //__COMMANDERKERNAEL_DRIVERS_PCI_VMWareSVGAII_HPP