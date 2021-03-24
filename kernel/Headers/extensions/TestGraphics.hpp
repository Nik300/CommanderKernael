#pragma once

#include <Headers/extensions/ext.hpp>

//libc
#include <libc/random.hpp>

namespace System{
    namespace Extensions{
        class TestGraphics: public System::Extensions::GraphicalExtension{
        private:
        public:
            CommanderKernael::Graphics::WindowContainer* SetupWindow(CommanderKernael::Drivers::VAL* video_device) override{
                return new CommanderKernael::Graphics::WindowContainer(0, 0, 100, 100, video_device, videoutils::GetRGB(255, 0, 0));
            }
            void main(CommanderKernael::Graphics::WindowContainer* container, System::Collections::Generic::List<string>* args) override{
                static int xi = 1;
                static int yi = 1;
                static uint8_t r = 30;
                static uint8_t g = 30;
                static uint8_t b = 30;

                static uint8_t ar = 10;
                static uint8_t ag = 20;
                static uint8_t ab = 30;

                static bool rd = false;
                static bool gd = false;
                static bool bd = false;

                while (true){
                    container->BaseX += xi;
                    container->BaseY += yi;
                    if (container->BaseX + container->width == 800){
                        System::Random::Rand random;
                        container->color = random.RandInt(0xFFFFFF00);

                        container->clear();

                        xi = -1;
                    }
                    if (container->BaseX == 0){
                        System::Random::Rand random;
                        container->color = random.RandInt(0xFFFFFF00);

                        container->clear();
                        
                        xi = 1;
                    }

                        
                    if (container->BaseY + container->height == 600){
                        System::Random::Rand random;
                        container->color = random.RandInt(0xFFFFFF00);

                        container->clear();

                        yi = -1;
                    }
                    if (container->BaseY == 0){
                        System::Random::Rand random;
                        container->color = random.RandInt(0xFFFFFF00);

                        container->clear();

                        yi = 1;
                    }
                    

                    container->color = videoutils::GetRGB(r, g, b);

                    container->refresh();
                }
            }
        };
    }
}