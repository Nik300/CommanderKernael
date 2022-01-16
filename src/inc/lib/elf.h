#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#define ARCH "i386"

#include <stdint.h>
#include <stddef.h>
#include <std/stddecl.h>

#ifndef __cplusplus
#include <stdbool.h>
#endif

typedef enum elf32_class
{
	ELFCLASS_NONE = 0,
	ELFCLASS_32 = 1,
	ELFCLASS_64 = 2
} elf32_class_t;
typedef enum elf32_endianness
{
	ELFDATA_2LSB = 1,
	ELFDATA_2MSB = 2
} elf32_endianness_t;
typedef enum elf32_osabi
{
	ELFOSABI_NONE = 0,
	ELFOSABI_HPUX = 1,
	ELFOSABI_NETBSD = 2,
	ELFOSABI_LINUX = 3,
	ELFOSABI_HURD = 4,
	ELFOSABI_SOLARIS = 6,
	ELFOSABI_AIX = 7,
	ELFOSABI_IRIX = 8,
	ELFOSABI_FREEBSD = 9,
	ELFOSABI_TRU64 = 10,
	ELFOSABI_MODESTO = 11,
	ELFOSABI_OPENBSD = 12,
	ELFOSABI_OPENVMS = 13,
	ELFOSABI_NSK = 14,
	ELFOSABI_AROS = 15,
	ELFOSABI_FENIXOS = 16,
	ELFOSABI_CLOUDABI = 17,
	ELFOSABI_OPENVOS = 18,
	ELFOSABI_ARM_AEABI = 64,
	ELFOSABI_ARM = 97,
	ELFOSABI_STANDALONE = 255
} elf32_osabi_t;
typedef enum elf32_machine {
	EM_NONE = 0,
	EM_M32 = 1,
	EM_SPARC = 2,
	EM_386 = 3,
	EM_68K = 4,
	EM_88K = 5,
	EM_860 = 7,
	EM_MIPS = 8,
	EM_S370 = 9,
	EM_MIPS_RS3_LE = 10,
	EM_PARISC = 15,
	EM_VPP500 = 17,
	EM_SPARC32PLUS = 18,
	EM_960 = 19,
	EM_PPC = 20,
	EM_PPC64 = 21,
	EM_S390 = 22,
	EM_V800 = 36,
	EM_FR20 = 37,
	EM_RH32 = 38,
	EM_RCE = 39,
	EM_ARM = 40,
	EM_FAKE_ALPHA = 41,
	EM_SH = 42,
	EM_SPARCV9 = 43,
	EM_TRICORE = 44,
	EM_ARC = 45,
	EM_H8_300 = 46,
	EM_H8_300H = 47,
	EM_H8S = 48,
	EM_H8_500 = 49,
	EM_IA_64 = 50,
	EM_MIPS_X = 51,
	EM_COLDFIRE = 52,
	EM_68HC12 = 53,
	EM_MMA = 54,
	EM_PCP = 55,
	EM_NCPU = 56,
	EM_NDR1 = 57,
	EM_STARCORE = 58,
	EM_ME16 = 59,
	EM_ST100 = 60,
	EM_TINYJ = 61,
	EM_AMD_X86_64 = 62,
	EM_TMS320C6X = 63,
	EM_MCST = 64,
	EM_AARCH64 = 183,
	EM_RISC_V = 243,
	EM_BARKPE = 248,
	EM_WDC_CPC = 249
} elf32_machine_t;

typedef enum elf32_type
{
	ET_NONE = 0,
	ET_REL = 1,
	ET_EXEC = 2,
	ET_DYN = 3,
	ET_CORE = 4
} elf32_type_t;

typedef struct elf32_ident
{
	char magic[4];
	elf32_class_t arch : 8;
	elf32_endianness_t data : 8;
	uint8_t version;
	elf32_osabi_t osabi : 8;
	uint8_t abiversion;
	uint8_t pading[7];
} __packed elf32_ident_t;
typedef struct elf32_prog_hdr
{
	uint32_t type;
	uint32_t offset;
	uint32_t vaddr;
	uint32_t paddr;
	uint32_t filesz;
	uint32_t memsz;
	uint32_t flags;
	uint32_t align;
} __packed elf32_prog_hdr_t;
typedef struct elf32_section_hdr
{
	uint32_t name;
	uint32_t type;
	uint32_t flags;
	uint32_t addr;
	uint32_t offset;
	uint32_t size;
	uint32_t link;
	uint32_t info;
	uint32_t addralign;
	uint32_t entsize;
} __packed elf32_section_hdr_t;
typedef struct elf32_header
{
	elf32_ident_t identifier;
	elf32_type_t type : 16;
	elf32_machine_t machine : 16;
	uint32_t version;
	void *entry;
	elf32_prog_hdr_t *phdr_table;
	elf32_section_hdr_t *shdr_table;
	uint32_t flags;
	uint16_t header_size;
	uint16_t phdr_size;
	uint16_t phdr_count;
	uint16_t shdr_size;
	uint16_t shdr_count;
	uint16_t shstr_index;
} __packed elf32_header_t;

bool elf32_validate_header(void *header);
uint32_t elf32_load(void* data, size_t size, uint8_t privilede);
void elf32_unload();

#ifdef __cplusplus
}
namespace System::Elf::x86_i386
{
	bool Validate(void *header);
	void *Load(void* data, size_t size);

	elf32_section_hdr_t* GetSection(elf32_header_t *header, const char *name);
	char* GetSectionName(elf32_header_t *header, uint32_t index);
	uint32_t GetSectionSize(elf32_header_t *header, uint32_t index);
	uint32_t GetSectionOffset(elf32_header_t *header, uint32_t index);

	uintptr_t GetPointerToSymbol(elf32_header_t *header, const char *name);
}
#endif