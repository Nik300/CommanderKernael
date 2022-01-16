#pragma once
#include <stdint.h>
#include <std/stdio.h>
    namespace CPU
    {

        void GetCPUInfo(uint32_t reg, uint32_t* eax, uint32_t* ebx, uint32_t* ecx, uint32_t* edx);
        void Detect();
        bool IsSSE();
        bool IsMMX();
    }