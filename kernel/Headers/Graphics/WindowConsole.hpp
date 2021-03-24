//Copyright Nicola Leone Ciardi (C)2020
//This file has been licensed by GNU-GPL
//All rights reserved. (C)2020
//Only contributors are authorized to modify/redistribute
//these files

//See CONTRIBUTORS.TXT for more infos

#ifndef __COMMANDERKERNAEL_GRAPHICS_WINDOWCONSOLE_HPP
#define __COMMANDERKERNAEL_GRAPHICS_WINDOWCONSOLE_HPP

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
#include <Headers/Graphics/GraphicsManager.hpp>
#include <Headers/Graphics/FontsSys.hpp>

using namespace CommanderKernael;
using namespace core;
using namespace Drivers;
using namespace CommanderKernael::utilities::ASCII_UTIL;
using namespace CommanderKernael::core::HAL::supports::PCI;
using namespace CommanderKernael::Drivers;

namespace CommanderKernael{
	namespace Graphics{
        class WindowConsole: public Console{
		private:
			WindowContainer* container;
			ConsoleColor Fcolor = ConsoleColor::White;
			ConsoleColor Bcolor = ConsoleColor::Black;
		public:
			WindowConsole(WindowContainer* container): Console() { this->container = container; }
			~WindowConsole() {}
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
				container->clear(CommanderKernael::utilities::videoutils::Get32Color(Bcolor));
				this->x = 0;
				this->y = 0;
			}
			virtual void printf(String text) {
				string t = text;
				Font* fonts = FromString(t);
				for (int i = 0; i < t.lenght(); i++, x += 5){
					if (FontsSystem::Render(container, fonts[i], x, y, CommanderKernael::utilities::videoutils::Get32Color(Fcolor), CommanderKernael::utilities::videoutils::Get32Color(Bcolor))) {
						y += 11;
						x = -5;
					}
					if (x >= container->width - 5){
						y += 11;
						x = -5;
					}
				}
                //container->device->Render();
			}
			virtual void printf(char character) {
				Font f;
				f.pattern = FromChar(character);
				if (FontsSystem::Render(container, f, x, y, CommanderKernael::utilities::videoutils::Get32Color(Fcolor), CommanderKernael::utilities::videoutils::Get32Color(Bcolor))) {
					y += 11;
					x = -5;
				}
				if (x >= container->width - 5){
					y += 11;
					x = -5;
				}
				x += 5;
                //container->device->Render();
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
					if (FontsSystem::Render(container, fonts[i], x, y, CommanderKernael::utilities::videoutils::Get32Color(Fcolor), CommanderKernael::utilities::videoutils::Get32Color(Bcolor))) {
						y += 11;
						x = -5;
					}
					if (x >= container->width - 5){
						y += 11;
						x = -5;
					}
				}
                //container->device->Render();
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

#endif //__COMMANDERKERNAEL_GRAPHICS_WINDOWCONSOLE_HPP