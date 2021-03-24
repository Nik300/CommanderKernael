#ifndef COMMANDERKERNAEL_DRIVES_FILESYSTEMS_VFS_HPP
#define COMMANDERKERNAEL_DRIVES_FILESYSTEMS_VFS_HPP

#include <Headers/types.hpp>
#include <Headers/util.hpp>
#include <Headers/drivers/ata.hpp>
#include <Headers/stdio.hpp>

#include <libc/exceptions.hpp>

using namespace assembly::types;
using namespace CommanderKernael::utilities::memory;
using namespace CommanderKernael::utilities::ASCII_UTIL;
using namespace CommanderKernael::Drivers;
using namespace CommanderKernael::Drivers::stdio;
using namespace System::Exceptions;

namespace CommanderKernael {
	namespace Drives{
        namespace FileSystems{
            namespace Exceptions{
                class SectorReservedException: public Exception { public: SectorReservedException(): Exception("Selected sector is reserved"){} };
                class SectorCorruptedException: public Exception { public: SectorCorruptedException(): Exception("Selected sector is corrupted"){} };
                class PartitionFullException: public Exception { public: PartitionFullException(): Exception("This partition is full"){} };
                class PartitionNotSelected: public Exception { public: PartitionNotSelected(): Exception("No partition was selected") {} };
                class DirectoryNotFoundException: public Exception { public: DirectoryNotFoundException(string dirName): Exception("File or directory " + dirName + " was not found."){} };
            }
            class VFS{
            public:
                VFS(){}
                int partitions;
                virtual void Initialize(int sectors, bool quick = true){}
                virtual void Initialize(int startSector, int sectors, bool quick = true){}
                virtual void CreateTable(string TableName){}
                virtual void CreatePartition(string name, int startSector, int sizeInKB, string root, int posInSector = 0, bool hidden = false, bool perms = true, char type = 0, bool quick = true){}
                virtual bool PartIsRW(int partNum){}
                virtual bool PartIsHidden(int partNum){}
                virtual string GetPartID(int partNum){}
                virtual void WriteToPartition(int position, int partNum, void* data, int size) {}
                virtual void WriteToPartition(int partNum, void* data, int size) {}
                virtual void CreateDir(int partNum, string path) {}
                virtual string PartName(int partNum) {}
                virtual string GetRootDirName(int partNum) {}
                virtual string GetDirName(int partNum, string path) {}
            };
        }
    }
}

#endif //COMMANDERKERNAEL_DRIVES_FILESYSTEMS_VFS_HPP