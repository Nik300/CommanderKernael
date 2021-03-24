//Copyright Nicola Leone Ciardi (C)2020
//This file has been licensed by GNU-GPL
//All rights reserved. (C)2020
//Only contributors are authorized to modify/redistribute
//these files

//See CONTRIBUTORS.TXT for more infos

#ifndef __COMMANDERKERNAEL_DRIVERS_ATA_HPP
#define __COMMANDERKERNAEL_DRIVERS_ATA_HPP

#include <Headers/environment.hpp>
#include <Headers/drivers/driver.hpp>
#include <Headers/stdio.hpp>
#include <Headers/types.hpp>
#include <Headers/core.hpp>
#include <Headers/ports.hpp>
#include <Headers/util.hpp>

using namespace CommanderKernael::Drivers;
using namespace CommanderKernael::Drivers::stdio;
using namespace CommanderKernael::utilities::ASCII_UTIL;
using namespace CommanderKernael::utilities::memory;
using namespace CommanderKernael::core::memory;
using namespace assembly::ports;
using namespace assembly::types;

extern "C" {
    extern uint8_t identify();
    extern void ata_pio_read28(uint32_t LBA, uint8_t sectorcount, uint8_t* target);
    extern void ata_pio_read48(uint64_t LBA, uint16_t sectorcount, uint8_t* target);
    extern void ata_pio_write48(uint64_t LBA, uint16_t sectorcount, uint8_t* target);
    extern void ata_pio_read_len28(uint32_t LBA, int lenght, uint8_t* target);
}

namespace CommanderKernael{
    namespace Drivers{
        class AdvancedTechnologyAttachement{
        public:
            static uint8_t loadATA() {
                return identify();
            }
            static void Write(uint64_t sector, uint16_t sectorcount, void* data) {
                ata_pio_write48(sector, sectorcount, (uint8_t*)data);
            }
            static void Write(uint64_t sector, uint8_t data) {
                ata_pio_write48(sector, 1, &data);
            }
            static void Read(uint64_t sector, uint16_t sectorcount, void* target) {
                ata_pio_read48(sector, sectorcount, (uint8_t*)target);
            }
            template<typename T> static void Read(uint64_t sector, uint16_t sectorcount, MemoryBlock<T>* target) {
                T* dat = (T*)allocateMemory(512 * sectorcount);
                *target = MemoryBlock<T>(512 * sectorcount);
                ata_pio_read48(sector, sectorcount, (uint8_t*)dat);
                target->WriteAll(dat, 512 * sectorcount);
            }
            static void Read28(uint32_t sector, uint8_t sectorcount, void* target) {
                ata_pio_read28(sector, sectorcount, (uint8_t*)target);
            }
            static void Read28(uint32_t sector, int len, void* target) {
                ata_pio_read_len28(sector, len, (uint8_t*)target);
            }
        };
    }
}

#endif //__COMMANDERKERNAEL_DRIVERS_ATA_HPP