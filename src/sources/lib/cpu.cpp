#include <lib/cpu.hpp>
#define ECX_SSE3                        (1 << 0)    // Streaming SIMD Extensions 3
#define ECX_PCLMULQDQ                   (1 << 1)    // PCLMULQDQ Instruction
#define ECX_DTES64                      (1 << 2)    // 64-Bit Debug Store Area
#define ECX_MONITOR                     (1 << 3)    // MONITOR/MWAIT
#define ECX_DS_CPL                      (1 << 4)    // CPL Qualified Debug Store
#define ECX_VMX                         (1 << 5)    // Virtual Machine Extensions
#define ECX_SMX                         (1 << 6)    // Safer Mode Extensions
#define ECX_EST                         (1 << 7)    // Enhanced SpeedStep Technology
#define ECX_TM2                         (1 << 8)    // Thermal Monitor 2
#define ECX_SSSE3                       (1 << 9)    // Supplemental Streaming SIMD Extensions 3
#define ECX_CNXT_ID                     (1 << 10)   // L1 Context ID
#define ECX_FMA                         (1 << 12)   // Fused Multiply Add
#define ECX_CX16                        (1 << 13)   // CMPXCHG16B Instruction
#define ECX_XTPR                        (1 << 14)   // xTPR Update Control
#define ECX_PDCM                        (1 << 15)   // Perf/Debug Capability MSR
#define ECX_PCID                        (1 << 17)   // Process-context Identifiers
#define ECX_DCA                         (1 << 18)   // Direct Cache Access
#define ECX_SSE41                       (1 << 19)   // Streaming SIMD Extensions 4.1
#define ECX_SSE42                       (1 << 20)   // Streaming SIMD Extensions 4.2
#define ECX_X2APIC                      (1 << 21)   // Extended xAPIC Support
#define ECX_MOVBE                       (1 << 22)   // MOVBE Instruction
#define ECX_POPCNT                      (1 << 23)   // POPCNT Instruction
#define ECX_TSC                         (1 << 24)   // Local APIC supports TSC Deadline
#define ECX_AESNI                       (1 << 25)   // AESNI Instruction
#define ECX_XSAVE                       (1 << 26)   // XSAVE/XSTOR States
#define ECX_OSXSAVE                     (1 << 27)   // OS Enabled Extended State Management
#define ECX_AVX                         (1 << 28)   // AVX Instructions
#define ECX_F16C                        (1 << 29)   // 16-bit Floating Point Instructions
#define ECX_RDRAND                      (1 << 30)   // RDRAND Instruction

#define EDX_FPU                         (1 << 0)    // Floating-Point Unit On-Chip
#define EDX_VME                         (1 << 1)    // Virtual 8086 Mode Extensions
#define EDX_DE                          (1 << 2)    // Debugging Extensions
#define EDX_PSE                         (1 << 3)    // Page Size Extension
#define EDX_TSC                         (1 << 4)    // Time Stamp Counter
#define EDX_MSR                         (1 << 5)    // Model Specific Registers
#define EDX_PAE                         (1 << 6)    // Physical Address Extension
#define EDX_MCE                         (1 << 7)    // Machine-Check Exception
#define EDX_CX8                         (1 << 8)    // CMPXCHG8 Instruction
#define EDX_APIC                        (1 << 9)    // APIC On-Chip
#define EDX_SEP                         (1 << 11)   // SYSENTER/SYSEXIT instructions
#define EDX_MTRR                        (1 << 12)   // Memory Type Range Registers
#define EDX_PGE                         (1 << 13)   // Page Global Bit
#define EDX_MCA                         (1 << 14)   // Machine-Check Architecture
#define EDX_CMOV                        (1 << 15)   // Conditional Move Instruction
#define EDX_PAT                         (1 << 16)   // Page Attribute Table
#define EDX_PSE36                       (1 << 17)   // 36-bit Page Size Extension
#define EDX_PSN                         (1 << 18)   // Processor Serial Number
#define EDX_CLFLUSH                     (1 << 19)   // CLFLUSH Instruction
#define EDX_DS                          (1 << 21)   // Debug Store
#define EDX_ACPI                        (1 << 22)   // Thermal Monitor and Software Clock Facilities
#define EDX_MMX                         (1 << 23)   // MMX Technology
#define EDX_FXSR                        (1 << 24)   // FXSAVE and FXSTOR Instructions
#define EDX_SSE                         (1 << 25)   // Streaming SIMD Extensions
#define EDX_SSE2                        (1 << 26)   // Streaming SIMD Extensions 2
#define EDX_SS                          (1 << 27)   // Self Snoop
#define EDX_HTT                         (1 << 28)   // Multi-Threading
#define EDX_TM                          (1 << 29)   // Thermal Monitor
#define EDX_PBE                         (1 << 31)   // Pending Break Enable

// ------------------------------------------------------------------------------------------------
// Extended Function 0x01

#define EDX_SYSCALL                     (1 << 11)   // SYSCALL/SYSRET
#define EDX_XD                          (1 << 20)   // Execute Disable Bit
#define EDX_1GB_PAGE                    (1 << 26)   // 1 GB Pages
#define EDX_RDTSCP                      (1 << 27)   // RDTSCP and IA32_TSC_AUX
#define EDX_64_BIT                      (1 << 29)   // 64-bit Architecture

    namespace CPU
    {
        using namespace System::IO;
        bool is_sse, is_mmx;
        // fetch cpu info from bios using inline assembly
        void GetCPUInfo(uint32_t reg, uint32_t* eax, uint32_t* ebx, uint32_t* ecx, uint32_t* edx)
        {
            __asm__ __volatile__("cpuid"
            : "=a" (*eax), "=b" (*ebx), "=c" (*ecx), "=d" (*edx)
            : "0" (reg));
        }
        
        // detect processor and get information
        void Detect()
        {
            // Register storage
            uint32_t eax, ebx, ecx, edx;

            // Function 0x00 - Vendor-ID and Largest Standard Function
            uint32_t largestStandardFunc;
            char vendor[13];
            GetCPUInfo(0, &largestStandardFunc, (uint32_t *)(vendor + 0), (uint32_t *)(vendor + 8), (uint32_t *)(vendor + 4));
            vendor[12] = '\0';
            Console::Write("CPU Vendor: ");
            Console::WriteLine(vendor);

            // Function 0x01 - Feature Information
            if (largestStandardFunc >= 0x01)
            {
                GetCPUInfo(0x01, &eax, &ebx, &ecx, &edx);

                Console::Write("Features:");

                if (edx & EDX_PSE)      Console::Write(" PSE");
                if (edx & EDX_PAE)      Console::Write(" PAE");
                if (edx & EDX_APIC)     Console::Write(" APIC");
                if (edx & EDX_MTRR)     Console::Write(" MTRR");

                Console::Write("\n");

                Console::Write("Instructions: ");

                if (edx & EDX_MMX)      { Console::Write(" MMX"); is_mmx = true; }
                if (edx & EDX_TSC)      Console::Write(" TSC");
                if (edx & EDX_MSR)      Console::Write(" MSR");
                if (edx & EDX_SSE)      { Console::Write(" SSE"); is_sse = true; }
                if (edx & EDX_SSE2)     Console::Write(" SSE2");
                if (ecx & ECX_SSE3)     Console::Write(" SSE3");
                if (ecx & ECX_SSSE3)    Console::Write(" SSSE3");
                if (ecx & ECX_SSE41)    Console::Write(" SSE41");
                if (ecx & ECX_SSE42)    Console::Write(" SSE42");
                if (ecx & ECX_AVX)      Console::Write(" AVX");
                if (ecx & ECX_F16C)     Console::Write(" F16C");
                if (ecx & ECX_RDRAND)   Console::Write(" RDRAND");

                Console::Write("\n");
            }

            // Extended Function 0x00 - Largest Extended Function
            uint32_t largestExtendedFunc;
            GetCPUInfo(0x80000000, &largestExtendedFunc, &ebx, &ecx, &edx);

            // Extended Function 0x01 - Extended Feature Bits
            if (largestExtendedFunc >= 0x80000001)
            {
                GetCPUInfo(0x80000001, &eax, &ebx, &ecx, &edx);

                if (edx & EDX_64_BIT)
                {
                    Console::WriteLine("64-bit Architecture");
                }
            }

            // Extended Function 0x02-0x04 - Processor Name / Brand String
            if (largestExtendedFunc >= 0x80000004)
            {
                char name[48];
                GetCPUInfo(0x80000002, (uint32_t*)(name +  0), (uint32_t *)(name +  4), (uint32_t *)(name +  8), (uint32_t *)(name + 12));
                GetCPUInfo(0x80000003, (uint32_t *)(name + 16), (uint32_t *)(name + 20), (uint32_t *)(name + 24), (uint32_t *)(name + 28));
                GetCPUInfo(0x80000004, (uint32_t *)(name + 32), (uint32_t *)(name + 36), (uint32_t *)(name + 40), (uint32_t *)(name + 44));

                // Processor name is right justified with leading spaces
                const char *p = name;
                while (*p == ' ')
                {
                    ++p;
                }

                Console::Write("CPU Name: "); 
                Console::WriteLine((char*)p);
            }
        }

        bool IsSSE() { return is_sse; }
        bool IsMMX() { return is_mmx; }
    }