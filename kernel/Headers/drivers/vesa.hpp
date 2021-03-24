//Copyright Nicola Leone Ciardi (C)2020
//This file has been licensed by GNU-GPL
//All rights reserved. (C)2020
//Only contributors are authorized to modify/redistribute
//these files

//See CONTRIBUTORS.TXT for more infos

#ifndef __COMMANDERKERNAEL_DRIVERS_VESA_HPP
#define __COMMANDERKERNAEL_DRIVERS_VESA_HPP

#include <Headers/environment.hpp>
#include <Headers/drivers/driver.hpp>
#include <Headers/stdio.hpp>
#include <Headers/types.hpp>
#include <Headers/core.hpp>
#include <Headers/ports.hpp>
#include <Headers/util.hpp>
#include <Headers/Graphics/VideoAbstractionLayer.hpp>
#include <Headers/Graphics/VideoUtils.hpp>
#include <Headers/Graphics/FontsSys.hpp>
#include <Headers/multitasking.hpp>

#include <libc/lock.hpp>

using namespace CommanderKernael::Drivers;
using namespace CommanderKernael::Drivers::stdio;
using namespace CommanderKernael::utilities::ASCII_UTIL;
using namespace CommanderKernael::utilities::memory;
using namespace assembly::ports;
using namespace assembly::types;
using namespace CommanderKernael::core;
using namespace CommanderKernael::Graphics::Fonts;
using namespace CommanderKernael::Graphics;
using namespace CommanderKernael::multitasking;

using namespace System;

#define LNG_PTR(seg, off) ((seg << 4) | off)
#define REAL_PTR(arr) LNG_PTR(arr[1], arr[0])
#define SEG(addr) (((uint32_t)addr >> 4) & 0xF000)
#define OFF(addr) ((uint32_t)addr & 0xFFFF)

#define CHAR_ROW 16
#define CHAR_COLUMN 8
#define CHARLEN (CHAR_ROW * CHAR_COLUMN) // 8 columns * 16 rows
#define CHARCOUNT 94 // 94 printable characters

#define CHAROFF(c) ((c - 32) * CHARLEN)

namespace CommanderKernael {
	namespace main {
		extern TaskManager* taskManager;
        extern GraphicsManager* Manager;
		extern VAL* vidModeDev;
	}
	namespace Drivers {
		namespace VideoBios {
			bool isVesaEnabled = false;
			bool CrenderA = false;
			bool CrenderB = false;
			bool CrenderC = false;
			bool CrenderD = false;
			bool CrenderE = false;
			bool CrenderF = false;
			bool CrenderG = false;
			bool CrenderH = false;
			bool VesaRendered = false;
			lock vesaLock;

			struct VbeInfoBlock {
				char VbeSignature[4];             // == "VESA"
				uint16_t VbeVersion;                 // == 0x0300 for VBE 3.0
				uint16_t OemStringPtr[2];            // isa vbeFarPtr
				uint8_t Capabilities[4];
				uint16_t VideoModePtr[2];         // isa vbeFarPtr
				uint16_t TotalMemory;             // as # of 64KB blocks
			} __attribute__((packed));

			struct ModeInfoBlock {
				uint16_t attributes;
				uint8_t winA, winB;
				uint16_t granularity;
				uint16_t winsize;
				uint16_t segmentA, segmentB;
				uint16_t realFctPtr[2];
				uint16_t pitch; // bytes per scanline

				uint16_t Xres, Yres;
				uint8_t Wchar, Ychar, planes, bpp, banks;
				uint8_t memory_model, bank_size, image_pages;
				uint8_t reserved0;

				uint8_t red_mask, red_position;
				uint8_t green_mask, green_position;
				uint8_t blue_mask, blue_position;
				uint8_t rsv_mask, rsv_position;
				uint8_t directcolor_attributes;

				uint32_t physbase;  // your LFB (Linear Framebuffer) address ;)
				uint32_t reserved1;
				uint16_t reserved2;
			} __attribute__((packed));
			VbeInfoBlock vib;
			bool vibset = false;
			class Vesa: public VAL {
				friend class VesaConsole;
			private:
				CommanderKernael::core::memory::MemoryBlock<uint32_t>* buffer;
				CommanderKernael::core::memory::MemoryBlock<uint32_t>* finalBuffer;
				ModeInfoBlock mib;
				uint32_t size;
				void populate_vib() {
					if (vibset) return;
				    core::debug::send_string(SERIAL_PORT_A, "[Vesa] requesting bios to populate VIB...\n");
					VbeInfoBlock* locvib = (VbeInfoBlock*)0x80000;

					regs16_t regs;
					regs.ax = 0x4f00;
					regs.es = 0x8000;
					regs.di = 0x0;
					cpu::interrupts::call16<0x10>(&regs);
					vibset = true;
					vib = *locvib;
				    core::debug::send_string(SERIAL_PORT_A, "[Vesa] got video infos successfully.\n");
				}
			public:
				Vesa(): VAL(){}
				~Vesa(){}
				virtual void Disable(){
					if (isVesaEnabled){
						isVesaEnabled = false;
						SetMode(80, 25, 4);
						isVesaEnabled = false;
					}
				}
				virtual bool SetMode(uint16_t width, uint16_t height, uint8_t colordepth) {
					if (isVesaEnabled) return false;
				    core::debug::send_string(SERIAL_PORT_A, "[Vesa] Loading...\n");
					if (!vibset)
						populate_vib();
					regs16_t regs;

					ModeInfoBlock* locmib = (ModeInfoBlock*)(0x80000 + sizeof(VbeInfoBlock) + 512);

					uint16_t* modes = (uint16_t*)REAL_PTR(vib.VideoModePtr);

					uint16_t mode;

					bool set = false;
				    core::debug::send_string(SERIAL_PORT_A, "[Vesa] Looking for requested mode...\n");

					for (int i = 0; modes[i] != 0xFFFF; i++) {
						regs.ax = 0x4f01;
						regs.cx = modes[i];
						regs.es = SEG(locmib);
						regs.di = OFF(locmib);
						cpu::interrupts::call16<0x10>(&regs);

						if (locmib->Xres == width && locmib->Yres == height && locmib->bpp == colordepth) {
							mode = modes[i];
							set = true;
                            core::debug::send_string(SERIAL_PORT_A, "[Vesa] Found matching mode ");
                            core::debug::send_string(SERIAL_PORT_A, intToASCII(locmib->Xres));
                            core::debug::send_string(SERIAL_PORT_A, "x");
                            core::debug::send_string(SERIAL_PORT_A, intToASCII(locmib->Yres));
                            core::debug::send_string(SERIAL_PORT_A, "x");
                            core::debug::send_string(SERIAL_PORT_A, intToASCII(locmib->bpp));
                            core::debug::send_string(SERIAL_PORT_A, " Going to enable it.\n");
							break;
						}
                        core::debug::send_string(SERIAL_PORT_A, "[Vesa] Found mode ");
                        core::debug::send_string(SERIAL_PORT_A, intToASCII(locmib->Xres));
                        core::debug::send_string(SERIAL_PORT_A, "x");
                        core::debug::send_string(SERIAL_PORT_A, intToASCII(locmib->Yres));
                        core::debug::send_string(SERIAL_PORT_A, "x");
                        core::debug::send_string(SERIAL_PORT_A, intToASCII(locmib->bpp));
                        core::debug::send_string(SERIAL_PORT_A, " which is not compatible with the requested one. Moving to next mode.\n");
					}
					isVesaEnabled = set;
					if (!set) { core::debug::send_string(SERIAL_PORT_A, "[Vesa: Error] No matching mode found. Aborting!\n"); return false; }
					core::debug::send_string(SERIAL_PORT_A, "[Vesa] Setting up buffer...\n");
					buffer = new CommanderKernael::core::memory::MemoryBlock<uint32_t>((locmib->Yres * locmib->pitch)/4);
					finalBuffer = new CommanderKernael::core::memory::MemoryBlock<uint32_t>((locmib->Yres * locmib->pitch)/4);
					this->height = locmib->Yres;
					this->width = locmib->Xres;
					for (int i = 0; i < buffer->size; i++)
						buffer->Write(i, 0);
                    core::debug::send_string(SERIAL_PORT_A, "[Vesa] Buffer set up\n");

                    core::debug::send_string(SERIAL_PORT_A, "[Vesa] Enabling mode...\n");
					regs.ax = 0x4F02;
					regs.bx = mode | 0x4000;
					cpu::interrupts::call16<0x10>(&regs);
                    core::debug::send_string(SERIAL_PORT_A, "[Vesa] Mode enabled.\n");

					mib = *locmib;
                    core::debug::send_string(SERIAL_PORT_A, "[Vesa] Stored the ModeInfoBlock.\n");
                    core::debug::send_string(SERIAL_PORT_A, "[Vesa] Vesa loaded successfully\n");
					return true;
				}
				virtual void SetPixel(int x, int y, uint32_t color) {
					if (x >= mib.Xres) return;
					if (y >= mib.Yres) return;
					if (buffer->Read(mib.Xres * y + x) == color) return;
					buffer->Write(mib.Xres * y + x, color);
				}
				virtual void Render() {
					memcpy((uint32_t*)finalBuffer->address, (uint32_t*)mib.physbase, finalBuffer->size);
				}
				virtual void EnableThreadedRender(){
					#pragma region tasks
					ParametrizedTask<uint32_t*, uint32_t*, int>* renderA = new ParametrizedTask<uint32_t*, uint32_t*, int>(1, 
					[](uint32_t* buffaddr, uint32_t* vidaddr, int size){
						while (isVesaEnabled){
							vesaLock.Lock();
							VesaRendered = false;
							memcpy(buffaddr, vidaddr, size);
							CrenderA = true;
							vesaLock.Unlock();
						}
						CurrentThread->TState = ThreadState::Stopped;
					}, new string("Section_A_VESA_renderer"), (uint32_t*)buffer->address, (uint32_t*)finalBuffer->address, buffer->size/8);

					ParametrizedTask<uint32_t*, uint32_t*, int>* renderB = new ParametrizedTask<uint32_t*, uint32_t*, int>(1, 
					[](uint32_t* buffaddr, uint32_t* vidaddr, int size){
						while (isVesaEnabled){
							vesaLock.Lock();
							VesaRendered = false;
							memcpy(buffaddr+size, vidaddr+size, size);
							CrenderB = true;
							vesaLock.Unlock();
						}
						CurrentThread->TState = ThreadState::Stopped;
					}, new string("Section_B_VESA_renderer"), (uint32_t*)buffer->address, (uint32_t*)finalBuffer->address, buffer->size/8);
					
					ParametrizedTask<uint32_t*, uint32_t*, int>* renderC = new ParametrizedTask<uint32_t*, uint32_t*, int>(1, 
					[](uint32_t* buffaddr, uint32_t* vidaddr, int size){
						while (isVesaEnabled){
							vesaLock.Lock();
							VesaRendered = false;
							memcpy(buffaddr+(size*2), vidaddr + (size*2), size);
							CrenderC = true;
							vesaLock.Unlock();
						}
						CurrentThread->TState = ThreadState::Stopped;
					}, new string("Section_C_VESA_renderer"), (uint32_t*)buffer->address, (uint32_t*)finalBuffer->address, buffer->size/8);
					
					ParametrizedTask<uint32_t*, uint32_t*, int>* renderD = new ParametrizedTask<uint32_t*, uint32_t*, int>(1, 
					[](uint32_t* buffaddr, uint32_t* vidaddr, int size){
						while (isVesaEnabled){
							vesaLock.Lock();
							VesaRendered = false;
							memcpy(buffaddr+(size*3), vidaddr+(size*3), size);
							CrenderD = true;
							vesaLock.Unlock();
						}
						CurrentThread->TState = ThreadState::Stopped;
					}, new string("Section_D_VESA_renderer"), (uint32_t*)buffer->address, (uint32_t*)finalBuffer->address, buffer->size/8);
					
					ParametrizedTask<uint32_t*, uint32_t*, int>* renderE = new ParametrizedTask<uint32_t*, uint32_t*, int>(1, 
					[](uint32_t* buffaddr, uint32_t* vidaddr, int size){
						while (isVesaEnabled){
							vesaLock.Lock();
							VesaRendered = false;
							memcpy(buffaddr+(size*4), vidaddr+(size*4), size);
							CrenderE = true;
							vesaLock.Unlock();
						}
						CurrentThread->TState = ThreadState::Stopped;
					}, new string("Section_E_VESA_renderer"), (uint32_t*)buffer->address, (uint32_t*)finalBuffer->address, buffer->size/8);
					
					ParametrizedTask<uint32_t*, uint32_t*, int>* renderF = new ParametrizedTask<uint32_t*, uint32_t*, int>(1, 
					[](uint32_t* buffaddr, uint32_t* vidaddr, int size){
						while (isVesaEnabled){
							vesaLock.Lock();
							VesaRendered = false;
							memcpy(buffaddr+(size*5), vidaddr+(size*5), size);
							CrenderF = true;
							vesaLock.Unlock();
						}
						CurrentThread->TState = ThreadState::Stopped;
					}, new string("Section_F_VESA_renderer"), (uint32_t*)buffer->address, (uint32_t*)finalBuffer->address, buffer->size/8);
					
					ParametrizedTask<uint32_t*, uint32_t*, int>* renderG = new ParametrizedTask<uint32_t*, uint32_t*, int>(1, 
					[](uint32_t* buffaddr, uint32_t* vidaddr, int size){
						while (isVesaEnabled){
							vesaLock.Lock();
							VesaRendered = false;
							memcpy(buffaddr+(size*6), vidaddr+(size*6), size);
							CrenderG = true;
							vesaLock.Unlock();
						}
						CurrentThread->TState = ThreadState::Stopped;
					}, new string("Section_G_VESA_renderer"), (uint32_t*)buffer->address, (uint32_t*)finalBuffer->address, buffer->size/8);
					
					ParametrizedTask<uint32_t*, uint32_t*, int>* renderH = new ParametrizedTask<uint32_t*, uint32_t*, int>(1, 
					[](uint32_t* buffaddr, uint32_t* vidaddr, int size){
						while (isVesaEnabled){
							vesaLock.Lock();
							VesaRendered = false;
							memcpy(buffaddr+(size*7), vidaddr+(size*7), size);
							CrenderH = true;
							vesaLock.Unlock();
						}
						CurrentThread->TState = ThreadState::Stopped;
					}, new string("Section_H_VESA_renderer"), (uint32_t*)buffer->address, (uint32_t*)finalBuffer->address, buffer->size/8);
					
					ParametrizedTask<uint32_t*, uint32_t*, int>* render = new ParametrizedTask<uint32_t*, uint32_t*, int>(1, 
					[](uint32_t* buffaddr, uint32_t* vidaddr, int size){
						while (isVesaEnabled){
							if (CrenderA && CrenderB && CrenderC && CrenderD && CrenderE && CrenderF && CrenderG && CrenderH){
								__asm__ volatile("cli");
								if (!main::Manager->RenderDone){
									main::Manager->Update(main::vidModeDev);
									main::Manager->RenderDone = true;
									continue;
								}
								main::Manager->RenderDone = false;
								vesaLock.Lock();
								memcpy(buffaddr, vidaddr, size);
								VesaRendered = true;
								vesaLock.Unlock();
								__asm__ volatile("sti");
							}
						}
						CurrentThread->TState = ThreadState::Stopped;
					}, new string("General_VESA_renderer"), (uint32_t*)finalBuffer->address, (uint32_t*)mib.physbase, finalBuffer->size);
					#pragma endregion

					taskManager->AddTask(renderA);
					taskManager->AddTask(renderB);
					taskManager->AddTask(renderC);
					taskManager->AddTask(renderD);
					taskManager->AddTask(renderE);
					taskManager->AddTask(renderF);
					taskManager->AddTask(renderG);
					taskManager->AddTask(renderH);
					taskManager->AddTask(render);
				}
			};
			class VesaConsole : public Console {
			private:
				Vesa* device;
				ConsoleColor Fcolor = (ConsoleColor)255;
				ConsoleColor Bcolor = (ConsoleColor)0;
			public:
				VesaConsole(Vesa* dev) : Console() { device = dev; }
				~VesaConsole(){}

				int x = 0, y = 0;

				virtual void disableCursor() {
				}
				virtual void setForegroundColor(ConsoleColor color) {
					Fcolor = color;
				}
				virtual void setBackgroundColor(ConsoleColor color) {
					Bcolor = color;
				}
				virtual void resetColors() {
					setForegroundColor(ConsoleColor::White);
					setForegroundColor(ConsoleColor::Black);
				}
				virtual ConsoleColor getForegroundColor() {
					return Fcolor;
				}
				virtual ConsoleColor getBackgroundColor() {
					return Bcolor;
				}
				virtual void clear() {
					for (int x = 0; x < device->mib.Xres; x++)
						for (int y = 0; y < device->mib.Yres; y++)
							device->SetPixel(x, y, CommanderKernael::utilities::videoutils::Get32Color(Bcolor));
					device->Render();
					this->x = 0;
					this->y = 0;
				}
				virtual void printf(String text) {
					string t = text;
					Font* fonts = FromString(t);
					for (int i = 0; i < t.lenght(); i++, x += 5){
						if (FontsSystem::Render(device, fonts[i], x, y, CommanderKernael::utilities::videoutils::Get32Color(Fcolor), CommanderKernael::utilities::videoutils::Get32Color(Bcolor))) {
							y += 11;
							x = -5;
						}
						if (x >= device->mib.Xres - 5){
								y += 11;
								x = -5;
						}
					}
					device->Render();
				}
				virtual void printf(char character) {
					Font f;
					f.pattern = FromChar(character);
					if (FontsSystem::Render(device, f, x, y, CommanderKernael::utilities::videoutils::Get32Color(Fcolor), CommanderKernael::utilities::videoutils::Get32Color(Bcolor))) {
						y += 11;
						x = -5;
					}
					if (x >= device->mib.Xres - 5){
						y += 11;
						x = -5;
					}
					x += 5;
					device->Render();
				}
				virtual void printf(bool boolean) {
					if (boolean) printf("true");
					else printf("false");
				}
				virtual void printf(int number) {
					printf(intToASCII(number));
				}
				virtual void printf(string text) {
					Font* fonts = FromString(text);
					for (int i = 0; i < text.lenght(); i++, x += 5){
						if (FontsSystem::Render(device, fonts[i], x, y, CommanderKernael::utilities::videoutils::Get32Color(Fcolor), CommanderKernael::utilities::videoutils::Get32Color(Bcolor))) {
							y += 11;
							x = -5;
						}
						if (x >= device->mib.Xres - 5){
								y += 11;
								x = -5;
						}
					}
					device->Render();
				}
				virtual void println() {
					y += 11;
					x = 0;
				}
				virtual void println(String text) {
					printf(text);
					y += 11;
					x = 0;
				}
				virtual void println(char character) {
					printf(character);
					y += 11;
					x = 0;
				}
				virtual void println(bool boolean) {
					printf(boolean);
					y += 11;
					x = 0;
				}
				virtual void println(int number) {
					println(intToASCII(number));
				}
				virtual void println(string text) {
					printf(text);
					y += 11;
					x = 0;
				}
				virtual void printf_at(String text, int x, int y) {
					this->x = x * 5;
					this->y = y * 11;
					printf(text);
					//this->x = x * 5;
				}
				virtual bool hasOwnership() {

				}
				virtual void takeOwnership() {

				}
				virtual int cursor_x() {
					return x/5;
				}
				virtual int cursor_y() {
					return y/11;
				}
				virtual void set_cursor_x(int x) {
					this->x = x*5;
				}
				virtual void set_cursor_y(int y) {
					this->y = y*11;
				}
				virtual void set_cursor_pos(int x, int y) {
					this->x = x*5;
					this->y = y*11;
				}
			};
		}
	}
}
#endif //__COMMANDERKERNAEL_DRIVERS_VESA_HPP
