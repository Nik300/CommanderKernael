#pragma once

#include <Headers/extensions/ext.hpp>

namespace System{
    namespace Extensions{
        class GraphicsExtension: public System::Extensions::GraphicalExtension{
        private:
        public:
            CommanderKernael::Graphics::WindowContainer* SetupWindow(CommanderKernael::Drivers::VAL* video_device) override{
                return new WindowContainer(0, 0, video_device->width, video_device->height, video_device, videoutils::GetRGB(0, 0, 0));
            }
            void main(CommanderKernael::Graphics::WindowContainer* container, System::Collections::Generic::List<string>* args) override{
                while (true){
                    container->refresh();
                }
            }
        };
    }
}