//Copyright Nicola Leone Ciardi (C)2020
//This file has been licensed by GNU-GPL
//All rights reserved. (C)2020
//Only contributors are authorized to modify/redistribute
//these files

//See CONTRIBUTORS.TXT for more infos

#ifndef __COMMANDERKERNAEL_GRAPHICS_FONTS_HPP
#define __COMMANDERKERNAEL_GRAPHICS_FONTS_HPP

#include <Headers/environment.hpp>
#include <Headers/drivers/driver.hpp>
#include <Headers/stdio.hpp>
#include <Headers/types.hpp>
#include <Headers/core.hpp>
#include <Headers/ports.hpp>
#include <Headers/util.hpp>
#include <Headers/Graphics/VideoAbstractionLayer.hpp>
#include <Headers/Graphics/GraphicsManager.hpp>

using namespace CommanderKernael::Drivers;
using namespace CommanderKernael::Drivers::stdio;
using namespace CommanderKernael::utilities::ASCII_UTIL;
using namespace CommanderKernael::utilities::memory;
using namespace assembly::ports;
using namespace assembly::types;
using namespace CommanderKernael::core;

namespace CommanderKernael {
	namespace Graphics {
		struct Font {
			bool* pattern;
		};
		class FontsSystem {
		public:
			static bool isN(Font f) {
				for (int i = 0; i < 28; i++) {
					if (!f.pattern[i]) return false;
				}
				return true;
			}

			//Video device directly
			static bool Render(VAL* device, Font font, int x, int y, uint32_t color) {
				if (isN(font)) return true;
				int current = 0;
				int ty = 0;
				bool first = false;
				for (int tx = 0, count = 0; current < 40; tx++, current++, count++)
				{
					if (count == 4)
					{
						ty++;
						count = 0;
						tx = 0;
					}
					if (font.pattern[current]) {
						if (!first) first = true;
						device->SetPixel(tx + x, ty + y, color);
					}
				}
				return false;
			}
			static bool Render(VAL* device, Font font, int x, int y, uint32_t fcolor, uint32_t bcolor) {
				if (isN(font)) return true;
				int current = 0;
				int ty = 0;
				bool first = false;
				for (int tx = 0, count = 0; current < 40; tx++, current++, count++)
				{
					if (count == 4)
					{
						ty++;
						count = 0;
						tx = 0;
					}
					if (font.pattern[current]) {
						if (!first) first = true;
						device->SetPixel(tx + x, ty + y, fcolor);
					} else device->SetPixel(tx + x, ty + y, bcolor);
				}
				return false;
			}

			//WindowContainer version
			static bool Render(WindowContainer* container, Font font, int x, int y, uint32_t color) {
				if (isN(font)) return true;
				int current = 0;
				int ty = 0;
				bool first = false;
				for (int tx = 0, count = 0; current < 40; tx++, current++, count++)
				{
					if (count == 4)
					{
						ty++;
						count = 0;
						tx = 0;
					}
					if (font.pattern[current]) {
						if (!first) first = true;
						container->DrawPoint(tx + x, ty + y, color);
					}
				}
				return false;
			}
			static bool Render(WindowContainer* container, Font font, int x, int y, uint32_t fcolor, uint32_t bcolor) {
				if (isN(font)) return true;
				int current = 0;
				int ty = 0;
				bool first = false;
				for (int tx = 0, count = 0; current < 40; tx++, current++, count++)
				{
					if (count == 4)
					{
						ty++;
						count = 0;
						tx = 0;
					}
					if (font.pattern[current]) {
						if (!first) first = true;
						container->DrawPoint(tx + x, ty + y, fcolor);
					} else container->DrawPoint(tx + x, ty + y, bcolor);
				}
				return false;
			}
		};
		namespace Fonts {
			#pragma region letters
			#pragma region CAPS
			bool A[40] = {
				false, true, true, false,
				true, false, false, true,
				true, false, false, true,
				true, true, true, true,
				true, false, false, true,
				true, false, false, true,
				true, false, false, true,
			};
			bool B[40] = {
				true, true, true, false,
				true, false, false, true,
				true, false, false, true,
				true, true, true, false,
				true, false, false, true,
				true, false, false, true,
				true, true, true, false,
			};
			bool C[40] = {
				false, true, true, true,
				true, false, false, false,
				true, false, false, false,
				true, false, false, false,
				true, false, false, false,
				true, false, false, false,
				false, true, true, true,
			};
			bool D[40] = {
				true, true, true, false,
				true, false, false, true,
				true, false, false, true,
				true, false, false, true,
				true, false, false, true,
				true, false, false, true,
				true, true, true, false,
			};
			bool E[40] = {
				true, true, true, true,
				true, false, false, false,
				true, false, false, false,
				true, true, true, false,
				true, false, false, false,
				true, false, false, false,
				true, true, true, true,
			};
			bool F[40] = {
				true, true, true, true,
				true, false, false, false,
				true, false, false, false,
				true, true, true, true,
				true, false, false, false,
				true, false, false, false,
				true, false, false, false,
			};
			bool G[40] = {
				false, true, true, true,
				true, false, false, false,
				true, false, false, false,
				true, false, true, true,
				true, false, false, true,
				true, false, false, true,
				false, true, true, true,
			};
			bool H[40] = {
				true, false, false, true,
				true, false, false, true,
				true, false, false, true,
				true, true, true, true,
				true, false, false, true,
				true, false, false, true,
				true, false, false, true,
			};
			bool I[40] = {
				true, true, true, false,
				false, true, false, false,
				false, true, false, false,
				false, true, false, false,
				false, true, false, false,
				false, true, false, false,
				true, true, true, false,
			};
			bool L[40] = {
				true, false, false, false,
				true, false, false, false,
				true, false, false, false,
				true, false, false, false,
				true, false, false, false,
				true, false, false, false,
				true, true, true, true,
			};
			bool M[40] = {
				true, false, false, true,
				true, true, true, true,
				true, true, true, true,
				true, false, false, true,
				true, false, false, true,
				true, false, false, true,
				true, false, false, true,
			};
			bool N[40] = {
				true, false, false, true,
				true, true, false, true,
				true, true, true, true,
				true, false, true, true,
				true, false, true, true,
				true, false, false, true,
				true, false, false, true,
			};
			bool J[40] = {
				true, true, true, true,
				false, false, false, true,
				false, false, false, true,
				false, false, false, true,
				true, false, false, true,
				true, false, false, true,
				false, true, true, false,
			};
			bool K[40] = {
				true, false, false, true,
				true, false, true, false,
				true, true, false, false,
				true, true, false, false,
				true, true, false, false,
				true, false, true, false,
				true, false, false, true,
			};
			bool O[40] = {
				false, true, true, false,
				true, false, false, true,
				true, false, false, true,
				true, false, false, true,
				true, false, false, true,
				true, false, false, true,
				false, true, true, false,
			};
			bool P[40] = {
				true, true, true, false,
				true, false, false, true,
				true, false, false, true,
				true, true, true, false,
				true, false, false, false,
				true, false, false, false,
				true, false, false, false,
			};
			bool Q[40] = {
				false, true, true, false,
				true, false, false, true,
				true, false, false, true,
				true, false, false, true,
				true, true, false, true,
				true, false, true, true,
				false, true, true, true,
			};
			bool R[40] = {
				true, true, true, false,
				true, false, false, true,
				true, false, false, true,
				true, true, true, false,
				true, true, false, false,
				true, false, true, false,
				true, false, false, true,
			};
			bool S[40] = {
				false, true, true, true,
				true, false, false, false,
				true, false, false, false,
				false, true, true, false,
				false, false, false, true,
				false, false, false, true,
				true, true, true, false,
			};
			bool T[40] = {
				true, true, true, false,
				false, true, false, false,
				false, true, false, false,
				false, true, false, false,
				false, true, false, false,
				false, true, false, false,
				false, true, false, false,
			};
			bool U[40] = {
				true, false, false, true,
				true, false, false, true,
				true, false, false, true,
				true, false, false, true,
				true, false, false, true,
				true, false, false, true,
				false, true, true, false,
			};
			bool V[40] = {
				true, false, true, false,
				true, false, true, false,
				true, false, true, false,
				true, false, true, false,
				true, false, true, false,
				true, false, true, false,
				false, true, false, false,
			};
			bool W[40] = {
				true, false, false, true,
				true, false, false, true,
				true, false, false, true,
				true, false, false, true,
				true, true, true, true,
				true, true, true, true,
				true, false, false, true,
			};
			bool X[40] = {
				true, false, false, true,
				true, false, false, true,
				false, true, true, false,
				false, true, true, false,
				false, true, true, false,
				true, false, false, true,
				true, false, false, true,
			};
			bool Y[40] = {
				true, false, true, false,
				true, false, true, false,
				true, false, true, false,
				false, true, false, false,
				false, true, false, false,
				false, true, false, false,
				false, true, false, false,
			};
			bool Z[40] = {
				true, true, true, true,
				false, false, true, false,
				false, false, true, false,
				false, true, false, false,
				false, true, false, false,
				true, false, false, false,
				true, true, true, true,
			};
			#pragma endregion
			#pragma region normal
			bool a[40] = {
				false, false, false, false,
				false, true, true, false,
				true, false, false, true,
				false, false, false, true,
				false, true, true, true,
				true, false, false, true,
				false, true, true, true,
			};
			bool b[40] = {
				true, false, false, false,
				true, false, false, false,
				true, false, false, false,
				true, true, true, false,
				true, false, false, true,
				true, false, false, true,
				true, true, true, false,
			};
			bool c[40] = {
				false, false, false, false,
				false, false, false, false,
				false, true, true, false,
				true, false, false, false,
				true, false, false, false,
				true, false, false, false,
				false, true, true, false,
			};
			bool d[40] = {
				false, false, false, true,
				false, false, false, true,
				false, false, false, true,
				false, true, true, true,
				true, false, false, true,
				true, false, false, true,
				false, true, true, true,
			};
			bool e[40] = {
				false, false, false, false,
				false, false, false, false,
				false, true, true, false,
				true, false, false, true,
				true, true, true, true,
				true, false, false, false,
				false, true, true, true,
			};
			bool f[40] = {
				false, false, true, true,
				false, true, false, false,
				false, true, false, false,
				true, true, true, false,
				false, true, false, false,
				false, true, false, false,
				false, true, false, false,
			};
			bool g[40] = {
				false, true, true, false,
				true, false, false, true,
				true, false, false, true,
				false, true, true, false,
				true, false, false, false,
				false, true, true, false,
				true, false, false, true,
				true, false, false, true,
				false, true, true, false,
				false, false, false, false,
			};
			bool h[40] = {
				true, false, false, false,
				true, false, false, false,
				true, false, false, false,
				true, true, true, false,
				true, false, false, true,
				true, false, false, true,
				true, false, false, true,
			};
			bool i[40] = {
				false, true, false, false,
				false, false, false, false,
				true, true, false, false,
				false, true, false, false,
				false, true, false, false,
				false, true, false, false,
				true, true, true, false,
			};
			bool l[40] = {
				true, true, false, false,
				false, true, false, false,
				false, true, false, false,
				false, true, false, false,
				false, true, false, false,
				false, true, false, false,
				true, true, true, false,
			};
			bool m[40] = {
				false, false, false, false,
				true, false, false, false,
				true, true, true, true,
				true, true, true, true,
				true, false, false, true,
				true, false, false, true,
				true, false, false, true,
			};
			bool n[40] = {
				false, false, false, false,
				false, false, false, false,
				true, false, true, false,
				true, true, false, true,
				true, false, false, true,
				true, false, false, true,
				true, false, false, true,
			};
			bool j[40] = {
				false, false, true, false,
				false, false, false, false,
				true, true, true, false,
				false, false, true, false,
				false, false, true, false,
				true, false, true, false,
				false, true, false, false,
			};
			bool k[40] = {
				true, false, false, false,
				true, false, false, false,
				true, false, true, true,
				true, true, false, false,
				true, true, false, false,
				true, false, true, false,
				true, false, false, true,
			};
			bool o[40] = {
				false, false, false, false,
				false, false, false, false,
				false, true, true, false,
				true, false, false, true,
				true, false, false, true,
				true, false, false, true,
				false, true, true, false,
			};
			bool p[40] = {
				false, false, false, false,
				false, false, false, false,
				true, true, true, false,
				true, false, false, true,
				true, false, false, true,
				true, true, true, false,
				true, false, false, false,
				true, false, false, false,
				true, false, false, false,
				false, false, false, false,
			};
			bool q[40] = {
				false, false, false, false,
				false, false, false, false,
				false, true, true, true,
				true, false, false, true,
				true, false, false, true,
				false, true, true, true,
				false, false, false, true,
				false, false, false, true,
				false, false, false, true,
				false, false, false, false,
			};
			bool r[40] = {
				false, false, false, false,
				true, false, false, false,
				true, false, true, true,
				true, true, false, false,
				true, false, false, false,
				true, false, false, false,
				true, false, false, false,
			};
			bool s[40] = {
				false, false, false, false,
				false, false, false, false,
				false, true, true, false,
				true, false, false, false,
				false, true, true, false,
				false, false, false, true,
				true, true, true, false,
			};
			bool t[40] = {
				false, true, false, false,
				true, true, true, true,
				false, true, false, false,
				false, true, false, false,
				false, true, false, false,
				false, true, false, false,
				false, false, true, true,
			};
			bool u[40] = {
				false, false, false, false,
				false, false, false, false,
				true, false, false, true,
				true, false, false, true,
				true, false, false, true,
				true, false, true, true,
				false, true, false, true,
			};
			bool v[40] = {
				false, false, false, false,
				false, false, false, false,
				true, false, true, false,
				true, false, true, false,
				true, false, true, false,
				true, false, true, false,
				false, true, false, false,
			};
			bool w[40] = {
				false, false, false, false,
				false, false, false, false,
				true, false, false, true,
				true, false, false, true,
				true, false, false, true,
				true, true, true, true,
				true, false, false, true,
			};
			bool x[40] = {
				false, false, false, false,
				false, false, false, false,
				true, false, false, true,
				false, true, true, false,
				false, true, true, false,
				false, true, true, false,
				true, false, false, true,
			};
			bool y[40] = {
				false, false, false, false,
				false, false, false, false,
				false, true, false, true,
				false, true, false, true,
				false, true, false, true,
				false, false, true, false,
				false, false, true, false,
				false, false, true, false,
				false, true, false, false,
				true, false, false, false,
			};
			bool z[40] = {
				false, false, false, false,
				false, false, false, false,
				true, true, true, true,
				false, false, true, false,
				false, true, false, false,
				true, false, false, false,
				true, true, true, true,
			};
			#pragma endregion
			#pragma endregion
			#pragma region special_chars
			bool point[40] = {
				false, false, false, false,
				false, false, false, false,
				false, false, false, false,
				false, false, false, false,
				false, false, false, false,
				false, true, true, false,
				false, true, true, false,
			};
			bool prc[40] = {
				false, true, false, true,
				true, false, true, true,
				false, true, true, false,
				false, false, true, false,
				false, true, false, false,
				false, true, true, false,
				true, true, false, true,
				true, false, true, false,
			};
			bool space[40] = {
				false, false, false, false,
				false, false, false, false,
				false, false, false, false,
				false, false, false, false,
				false, false, false, false,
				false, false, false, false,
				false, false, false, false,
			};
			bool apostrophe[40] = {
				false, true, false, false,
				false, true, false, false,
				false, true, false, false,
				false, false, false, false,
				false, false, false, false,
				false, false, false, false,
				false, false, false, false,
			};
			bool qm[40] = {
				true, true, true, false,
				false, false, false, true,
				false, false, false, true,
				false, true, true, false,
				false, true, false, false,
				false, false, false, false,
				false, true, false, false,
			};
			bool em[40] = {
				false, true, false, false,
				false, true, false, false,
				false, true, false, false,
				false, true, false, false,
				false, true, false, false,
				false, false, false, false,
				false, true, false, false,
			};
			bool nline[40] = {
				true, true, true, true,
				true, true, true, true,
				true, true, true, true,
				true, true, true, true,
				true, true, true, true,
				true, true, true, true,
				true, true, true, true,
			};
			bool hyph[40] = {
				false, false, false, false,
				false, false, false, false,
				false, false, false, false,
				true, true, true, true,
				false, false, false, false,
				false, false, false, false,
				false, false, false, false,
			};
			bool uscore[40] = {
				false, false, false, false,
				false, false, false, false,
				false, false, false, false,
				false, false, false, false,
				false, false, false, false,
				false, false, false, false,
				true, true, true, true,
			};
			bool colon[40] = {
				false, false, false, false,
				true, false, false, false,
				false, false, false, false,
				false, false, false, false,
				false, false, false, false,
				true, false, false, false,
				false, false, false, false,
			};
			bool coma[40] = {
				false, false, false, false,
				false, false, false, false,
				false, false, false, false,
				false, false, false, false,
				false, false, false, false,
				false, false, false, false,
				false, false, false, false,
				false, true, false, false,
				false, true, false, false,
				true, false, false, false,
			};
			#pragma endregion
			#pragma region brackets
			bool osquareb[40] = {
				true, true, false, false,
				true, false, false, false,
				true, false, false, false,
				true, false, false, false,
				true, false, false, false,
				true, false, false, false,
				true, true, false, false,
			};
			bool csquareb[40] = {
				false, false, true, true,
				false, false, false, true,
				false, false, false, true,
				false, false, false, true,
				false, false, false, true,
				false, false, false, true,
				false, false, true, true,
			};
			bool obracket[40] = {
				false, false, false, true,
				false, false, true, false,
				false, false, true, false,
				false, false, true, false,
				false, false, true, false,
				false, false, true, false,
				false, false, false, true,
			};
			bool cbracket[40] = {
				true, false, false, false,
				false, true, false, false,
				false, true, false, false,
				false, true, false, false,
				false, true, false, false,
				false, true, false, false,
				true, false, false, false,
			};
			bool ocurlyb[40] = {
				false, false, false, true,
				false, false, true, false,
				false, false, true, false,
				false, true, false, false,
				false, false, true, false,
				false, false, true, false,
				false, false, false, true,
			};
			bool ccurlyb[40] = {
				true, false, false, false,
				false, true, false, false,
				false, true, false, false,
				false, false, true, false,
				false, true, false, false,
				false, true, false, false,
				true, false, false, false,
			};
			bool ohtmlbrackets[40] = {
				true, false, false, false,
				false, true, false, false,
				false, false, true, false,
				false, false, false, true,
				false, false, true, false,
				false, true, false, false,
				true, false, false, false,
			};
			bool chtmlbrackets[40] = {
				false, false, false, true,
				false, false, true, false,
				false, true, false, false,
				true, false, false, false,
				false, true, false, false,
				false, false, true, false,
				false, false, false, true,
			};
			#pragma endregion
			#pragma region numbers
			bool _1[40] = {
				false, false, true, false,
				false, true, true, false,
				true, false, true, false,
				false, false, true, false,
				false, false, true, false,
				false, false, true, false,
				false, true, true, true,
			};
			bool _2[40] = {
				true, true, true, false,
				false, false, false, true,
				false, false, false, true,
				false, false, true, false,
				false, true, false, false,
				true, false, false, false,
				true, true, true, true,
			};
			bool _3[40] = {
				true, true, true, false,
				false, false, false, true,
				false, false, false, true,
				false, true, true, false,
				false, false, false, true,
				false, false, false, true,
				true, true, true, false,
			};
			bool _4[40] = {
				true, false, false, false,
				true, false, false, false,
				true, false, false, false,
				true, false, true, false,
				true, true, true, true,
				false, false, true, false,
				false, false, true, false,
			};
			bool _5[40] = {
				true, true, true, true,
				true, false, false, false,
				true, false, false, false,
				true, true, true, false,
				false, false, false, true,
				false, false, false, true,
				true, true, true, false,
			};
			bool _6[40] = {
				false, false, true, true,
				false, true, false, false,
				true, false, false, false,
				true, true, true, false,
				true, false, false, true,
				true, false, false, true,
				false, true, true, false,
			};
			bool _7[40] = {
				true, true, true, true,
				false, false, false, true,
				false, false, false, true,
				false, false, true, false,
				false, false, true, false,
				false, true, false, false,
				false, true, false, false,
			};
			bool _8[40] = {
				false, true, true, false,
				true, false, false, true,
				true, false, false, true,
				false, true, true, false,
				true, false, false, true,
				true, false, false, true,
				false, true, true, false,
			};
			bool _9[40] = {
				false, true, true, false,
				true, false, false, true,
				true, false, false, true,
				false, true, true, true,
				false, false, false, true,
				false, false, true, false,
				true, true, false, false,
			};
			bool _0[40] = {
				false, true, true, false,
				true, false, false, true,
				true, false, true, true,
				true, false, true, true,
				true, true, false, true,
				true, true, false, true,
				false, true, true, false,
			};
			#pragma endregion
			bool* FromChar(char c) {
				switch (c)
				{
				case 'A':
					return A;
				case 'a':
					return a;
				case 'B':
					return B;
				case 'b':
					return b;
				case 'C':
					return C;
				case 'c':
					return Fonts::c;
				case 'D':
					return D;
				case 'd':
					return d;
				case 'E':
					return E;
				case 'e':
					return e;
				case 'F':
					return F;
				case 'f':
					return f;
				case 'G':
					return G;
				case 'g':
					return g;
				case 'H':
					return H;
				case 'h':
					return h;
				case 'I':
					return I;
				case 'i':
					return i;
				case 'L':
					return L;
				case 'l':
					return l;
				case 'M':
					return M;
				case 'm':
					return m;
				case 'N':
					return N;
				case 'n':
					return n;
				case 'J':
					return J;
				case 'j':
					return j;
				case 'K':
					return K;
				case 'k':
				    return k;
				case 'O':
					return O;
				case 'o':
				    return o;
				case 'P':
					return P;
				case 'p':
					return p;
				case 'Q':
					return Q;
				case 'q':
					return q;
				case 'R':
					return R;
				case 'r':
					return r;
				case 'S':
					return S;
				case 's':
					return s;
				case 'T':
					return T;
				case 't':
					return t;
				case 'U':
					return U;
				case 'u':
					return u;
				case 'V':
					return V;
				case 'v':
					return v;
				case 'W':
					return W;
				case 'w':
					return w;
				case 'X':
					return X;
				case 'x':
					return x;
				case 'Y':
					return Y;
				case 'y':
					return y;
				case 'Z':
					return Z;
				case 'z':
					return z;
				case '.':
					return point;
				case '?':
					return qm;
				case '!':
					return em;
				case '\n':
					return nline;
				case ' ':
					return space;
				case '%':
					return prc;
				case '-':
					return hyph;
				case '_':
					return uscore;
				case ':':
					return colon;
				case ',':
					return coma;
				case '[':
					return osquareb;
				case ']':
					return csquareb;
				case '(':
					return obracket;
				case ')':
					return cbracket;
				case '{':
					return ocurlyb;
				case '}':
					return ccurlyb;
				case '<':
					return chtmlbrackets;
				case '>':
					return ohtmlbrackets;
				case '\'':
					return apostrophe;
				case '1':
					return _1;
				case '2':
					return _2;
				case '3':
					return _3;
				case '4':
					return _4;
				case '5':
					return _5;
				case '6':
					return _6;
				case '7':
					return _7;
				case '8':
					return _8;
				case '9':
					return _9;
				case '0':
					return _0;
				default:
					return qm;
				}
				return qm;
			}
			Font FontFromChar(char c) {
				Font f;
				f.pattern = FromChar(c);
				return f;
			}
			Font* FromString(string text) {
				Font* f = (Font*)CommanderKernael::core::memory::allocateMemory(text.lenght());
				for (int i = 0; i < text.lenght(); i++) {
					f[i] = FontFromChar(text._iterator[i]);
				}
				return f;
			}
		}
	}
}

#endif //__COMMANDERKERNAEL_GRAPHICS_FONTS_HPP
