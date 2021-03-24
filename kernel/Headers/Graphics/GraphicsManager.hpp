//Copyright Nicola Leone Ciardi (C)2020
//This file has been licensed by GNU-GPL
//All rights reserved. (C)2020
//Only contributors are authorized to modify/redistribute
//these files

//See CONTRIBUTORS.TXT for more infos

#ifndef __COMMANDERKERNAEL_GRAPHICS_HPP
#define __COMMANDERKERNAEL_GRAPHICS_HPP

#include <Headers/environment.hpp>
#include <Headers/core.hpp>
#include <Headers/drivers/keyboard.hpp>
#include <Headers/keyboards/languages.hpp>
#include <Headers/util.hpp>
#include <Headers/stdio.hpp>
#include <Headers/pci.hpp>
#include <Headers/drivers/vga.hpp>
#include <Headers/Graphics/VideoAbstractionLayer.hpp>
#include <Headers/Graphics/VideoUtils.hpp>

#include <libc/collections.hpp>

using namespace CommanderKernael;
using namespace core;
using namespace Drivers;
using namespace CommanderKernael::utilities::ASCII_UTIL;
using namespace CommanderKernael::core::HAL::supports::PCI;
using namespace CommanderKernael::Drivers;
using namespace System::Collections::Generic;

namespace CommanderKernael{
	namespace Drivers{
		namespace MouseDriver{
			static int32_t GetX();
			static int32_t GetY();
		}
	}
	namespace main{
		void RegisterWindow(int);
		void UpdateG();
	}
	namespace Graphics{
		class WindowContainer{
		friend class WindowConsole;
		friend class GraphicsManager;
		private:
			int initY;
			int initX;
			MemoryBlock<uint32_t>* pixels;
			VAL *device;

		public:
			int BaseX;
			int BaseY;
			int width;
			int height;
			int pitch;
			uint8_t pixelstride;
			uint32_t color;
			WindowContainer(int16_t x, int16_t y, int16_t width, int16_t height, VAL *device, uint32_t Color = 0){
				this->pixelstride = (32 | 7) >> 3;
				this->pitch = width * pixelstride;
				this->pixels = new MemoryBlock<uint32_t>(height * pitch);
				this->BaseX = x;
				this->BaseY = y;
				this->width = width;
				this->height = height;
				this->initY = y;
				this->initX = x;
				this->device = device;
				this->color = Color;
				clear();
				main::RegisterWindow((int)this);
			}
			~WindowContainer(){}
			
			void refresh(){
				for (int x = 0; x < width; x++)
					for (int y = 0; y < height; y++){
						DrawPoint(x, y, GetPoint(x, y));
					}
				UpdateG();
			}
			void clear(){
				for (int tmpX = 0; tmpX < width; tmpX++){
					for (int tmpY = 0; tmpY < height; tmpY++){
						DrawPoint(tmpX, tmpY, color);
					}
				}
			}
			void clear(uint32_t color){
				for (int tmpX = 0; tmpX < width; tmpX++){
					for (int tmpY = 0; tmpY < height; tmpY++){
						DrawPoint(tmpX, tmpY, color);
					}
				}
			}
			void DrawPoint(int x, int y, uint32_t color){
				//device->SetPixel(x + BaseX, y + BaseY, color);
				if (GetPoint(x, y) != color){
					pixels->Write(width*y+x, color);
				}
			}
			uint32_t GetPoint(int x, int y){
				return pixels->Read(width*y+x);
			}
		};
		class GraphicsManager{
		public:
			List<WindowContainer*> containers;
			bool RenderDone = false;
			void Update(VAL* device){
				//RenderDone = false;
				for (int i = 0; i < containers.Count; i++){
					auto window = ((WindowContainer*)containers[i]);
					//debug::send_string(SERIAL_PORT_A, "rendering\n");
					for (int x = 0; x < window->width; x++){
						for (int y = 0; y < window->height; y++){
							device->SetPixel(x + window->BaseX, y + window->BaseY, window->GetPoint(x, y));
						}
					}
                    device->RenderMouse(CommanderKernael::Drivers::MouseDriver::GetX(), CommanderKernael::Drivers::MouseDriver::GetY(), videoutils::GetRGB(255, 255, 255), videoutils::GetRGB(10, 10, 10));
				}
				RenderDone = true;
			}
		};
	}
}

#endif //__COMMANDERKERNAEL_GRAPHICS_HPP