#ifndef __COMMANDERKERNAEL_CORE_HAL_SUPPORTS_PCI
#define __COMMANDERKERNAEL_CORE_HAL_SUPPORTS_PCI

#include "Headers/stdio.hpp"
#include "Headers/util.hpp"

using namespace CommanderKernael::Drivers::stdio;
using namespace CommanderKernael::utilities::ASCII_UTIL;

namespace CommanderKernael{
    namespace core{
        namespace HAL{
            namespace supports{
                namespace PCI{
                    enum BARType{
                        MemoryMapping = 0,
                        InputOutput = 1
                    };
                    class BAR{
                    public:
                        bool prefetchable;
<<<<<<< HEAD
                        unsigned int address;
=======
                        unsigned char* address;
>>>>>>> 21ea778aacfb419a58f37175bff031c5c2721ccf
                        unsigned int size;
                        BARType type;
                    };

                    class PCIDevice{
                    public:
                        unsigned int portBase;
                        unsigned int interrupt;

                        unsigned short bus;
                        unsigned short device;
                        unsigned short function;

                        unsigned short vendor_id;
                        unsigned short device_id;

                        unsigned char class_id;
                        unsigned char subclass_id;
                        unsigned char interface_id;

                        unsigned char revision;

                        PCIDevice(){}
                        ~PCIDevice(){}
                    };
                    class PCIController{
                    private:
                        port32bit dataPort = port32bit(0xCFC);
                        port32bit commandPort = port32bit(0xCF8);

                    public:
                        PCIController(){}
                        ~PCIController(){}

                        unsigned int Read(PCIDevice* device, unsigned int registerOffset);
                        void Write(PCIDevice* device, unsigned int registerOffset, unsigned int data);
                        bool DeviceHasFunction(PCIDevice* device);

                        void SelectDrivers(Console console);
<<<<<<< HEAD
                        void EnableMemory(bool memory, PCIDevice* device);
                        PCIDevice GetDevice(unsigned short bus, unsigned short device, unsigned short function);
                        PCIDevice RequireDevice(unsigned short vendor_id, unsigned short device_id);
=======
                        PCIDevice GetDevice(unsigned short bus, unsigned short device, unsigned short function);
>>>>>>> 21ea778aacfb419a58f37175bff031c5c2721ccf
                        BAR GetBaseAddressReg(PCIDevice* device, unsigned short bar);
                    };
                }
            }
        }
    }
}

using namespace CommanderKernael::core::HAL::supports::PCI;

unsigned int PCIController::Read(PCIDevice* device, unsigned int registerOffset){
    unsigned int id =
        0x1 << 31
        | ((device->bus & 0xFF) << 16)
        | ((device->device & 0x1F) << 11)
        | ((device->function & 0x07) << 8)
        | (registerOffset & 0xFC);
    this->commandPort.write(id);
    unsigned int result = this->dataPort.read();
    return result >> (8*(registerOffset % 4));
}

void PCIController::Write(PCIDevice* device, unsigned int registerOffset, unsigned int data){
        unsigned int id =
        0x1 << 31
        | ((device->bus & 0xFF) << 16)
        | ((device->device & 0x1F) << 11)
        | ((device->function & 0x07) << 8)
        | (registerOffset & 0xFC);
    this->commandPort.write(id);
    this->dataPort.write(data);
}

bool PCIController::DeviceHasFunction(PCIDevice* device){
    return Read(device, 0x0E) & (1<<7);
}

void PCIController::SelectDrivers(Console console){
    console.takeOwnership();
    for (int bus = 0; bus < 8; bus++){
        for (int device = 0; device < 32; device++){
            PCIDevice dev = PCIDevice();
            dev.device = device;
            dev.bus = bus;
            int numFunctions = DeviceHasFunction(&dev) ? 1 : 8;
            for (int function = 0; function < numFunctions; function++){
                dev = GetDevice(bus, device, function);

                if (dev.vendor_id == 0x0000 || dev.vendor_id == 0xFFFF) continue;

                for (int barNum = 0; barNum < 6; barNum++){
                    BAR bar = GetBaseAddressReg(&dev, barNum);
                    if (bar.address && (bar.type == InputOutput)) dev.portBase = (unsigned int)bar.address;
                }

                switch (dev.vendor_id){
<<<<<<< HEAD
                    default:
                        console.printf("Unknown device found at (bus-device-function-vendor_id-device_id-revision): ");
=======
                    case 0x1234:
                        console.printf("RedHat device found at (bus-device-function): ");
>>>>>>> 21ea778aacfb419a58f37175bff031c5c2721ccf
                        console.printf((unsigned char)(dev.bus & 0xFF));
                        console.printf("-");
                        console.printf((unsigned char)(dev.device & 0xFF));
                        console.printf("-");
<<<<<<< HEAD
                        console.printf((unsigned char)(dev.function & 0xFF));
                        console.printf("-");
                        console.printf((unsigned char)((dev.vendor_id & 0xFF00) >> 8));
                        console.printf((unsigned char)(dev.vendor_id & 0xFF));
                        console.printf("-");
                        console.printf((unsigned char)((dev.device_id & 0xFF00) >> 8));
                        console.printf((unsigned char)(dev.device_id & 0xFF));
                        console.printf("-");
                        console.println((unsigned char)(dev.revision));
=======
                        console.println((unsigned char)(dev.function & 0xFF));
                        break;
                        break;
                    default:
                        console.printf("Unknown device found at (bus-device-function): ");
                        console.printf((unsigned char)(dev.bus & 0xFF));
                        console.printf("-");
                        console.printf((unsigned char)(dev.device & 0xFF));
                        console.printf("-");
                        console.println((unsigned char)(dev.function & 0xFF));
>>>>>>> 21ea778aacfb419a58f37175bff031c5c2721ccf
                        break;
                }
            }
        }
    }
}

<<<<<<< HEAD
PCIDevice PCIController::RequireDevice(unsigned short vendor_id, unsigned short device_id){
    for (int bus = 0; bus < 8; bus++){
        for (int device = 0; device < 32; device++){
            PCIDevice dev = PCIDevice();
            dev.device = device;
            dev.bus = bus;
            int numFunctions = DeviceHasFunction(&dev) ? 1 : 8;
            for (int function = 0; function < numFunctions; function++){
                dev = GetDevice(bus, device, function);

                if (dev.vendor_id == 0x0000 || dev.vendor_id == 0xFFFF) continue;

                for (int barNum = 0; barNum < 6; barNum++){
                    BAR bar = GetBaseAddressReg(&dev, barNum);
                    if (bar.address && (bar.type == InputOutput)) dev.portBase = (unsigned int)bar.address;
                }

                if (dev.device_id == device_id && dev.vendor_id == vendor_id) return dev;
            }
        }
    }
    return PCIDevice();
}

=======
>>>>>>> 21ea778aacfb419a58f37175bff031c5c2721ccf
BAR PCIController::GetBaseAddressReg(PCIDevice* device, unsigned short bar){
    BAR result;

    unsigned int headertype = Read(device, 0x0E) & 0x7F;
    int maxBARs = 6 - (4*headertype);
    if (bar >= maxBARs) return result;

    unsigned int bar_value = Read(device, 0x10 + 4*bar);
    result.type = (bar_value & 0x1) ? InputOutput : MemoryMapping;
    unsigned int temp;

    if (result.type == MemoryMapping){
<<<<<<< HEAD
        result.prefetchable = (unsigned short)((bar_value >> 3) & 0x01);
        switch ((bar_value >> 1) & 0x03)
        {
            case 0: 
                result.address = bar_value & 0xFFFFFFF0;
                break;
            case 1:
                result.address = bar_value & 0xFFFFFFF0;
=======
        switch ((bar_value >> 1) & 0x03)
        {
            case 0: //32bit
            case 1: //20bit
            case 2: //64bit
>>>>>>> 21ea778aacfb419a58f37175bff031c5c2721ccf
                break;
        }
    }
    else{
<<<<<<< HEAD
        result.address = bar_value & 0xFFFFFFFC;
=======
        result.address = (unsigned char*)(bar_value & ~0x3);
>>>>>>> 21ea778aacfb419a58f37175bff031c5c2721ccf
        result.prefetchable = false;
    }

    return result;
}

PCIDevice PCIController::GetDevice(unsigned short bus, unsigned short device, unsigned short function){
    PCIDevice result;

    result.bus = bus;
    result.device = device;
    result.function = function;

    result.vendor_id = Read(&result, 0x00);
    result.device_id = Read(&result, 0x02);

    result.class_id = Read(&result, 0x0b);
    result.subclass_id = Read(&result, 0x0a);
    result.interface_id = Read(&result, 0x09);

    result.revision = Read(&result, 0x08);
    result.interrupt = Read(&result, 0x3c);

    return result;
}
<<<<<<< HEAD

void PCIController::EnableMemory(bool memory, PCIDevice* device){
    unsigned short command = (unsigned short)(Read(device, 0x04));
    unsigned short flags = 0x0007;

    if (memory)
        command |= flags;
    else
        command &= (unsigned short)~flags;

    Write(device, 0x04, (unsigned int)command);
}
=======
>>>>>>> 21ea778aacfb419a58f37175bff031c5c2721ccf
#endif //__COMMANDERKERNAEL_CORE_HAL_SUPPORTS_PCI
