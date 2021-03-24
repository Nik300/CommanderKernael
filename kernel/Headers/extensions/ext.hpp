#ifndef __COMMANDERKERNAEL_EXT_HPP
#define __COMMANDERKERNAEL_EXT_HPP

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
#include <libc/collections.hpp>

#define CommanderKernael_Extension

namespace System{
    namespace Extensions{
        /*
            This determins the structure of a Graphical-based extension.

            -> SetupWindow: This function is called before initializing the thread, so to initialize the window that it's going to be used.
            -> Main: the main function which is going to be loaded in a thread (should be the actual program entry).
        */
        class GraphicalExtension{
        public:
            virtual CommanderKernael::Graphics::WindowContainer* SetupWindow(CommanderKernael::Drivers::VAL*)
            { return NULL; }

            virtual void main(CommanderKernael::Graphics::WindowContainer*, System::Collections::Generic::List<string>*)
            { return; }
        };

        class CLIExtension{
        public:
            virtual void main(System::Collections::Generic::List<string>*)
            { return; }
        };
    }
}
#endif