#ifndef COMMANDERKERNAEL_DRIVES_FILESYSTEMS_EXT2_HPP
#define COMMANDERKERNAEL_DRIVES_FILESYSTEMS_EXT2_HPP

#include <Headers/types.hpp>
#include <Headers/util.hpp>
#include <Headers/drivers/ata.hpp>
#include <Headers/stdio.hpp>

using namespace assembly::types;
using namespace CommanderKernael::utilities::memory;
using namespace CommanderKernael::utilities::ASCII_UTIL;
using namespace CommanderKernael::Drivers;
using namespace CommanderKernael::Drivers::stdio;

extern "C" {

#define VFS_NAME_MAX_SIZE 256

#define FS_MOUNTPOINT  0x01
#define FS_FILE        0x02
#define FS_DIRECTORY   0x04
#define FS_CHARDEVICE  0x08
#define FS_BLOCKDEVICE 0x10
#define FS_PIPE        0x20
#define FS_SYMLINK     0x40
#define FS_MASK        0xFE

    struct vfs_node;

    typedef struct vfs_node* inode_t;

    typedef struct _stat
    {
        uint64_t size;
    } stat_t;

    typedef struct dirent
    {
        char name[VFS_NAME_MAX_SIZE];
        inode_t inode;
    } dirent_t;

    typedef struct vfs_driver
    {
        uint64_t(*open)(inode_t inode, uint64_t mode);
        uint64_t(*close)(inode_t inode);
        uint64_t(*read)(inode_t inode, void* ptr, uint64_t length, uint64_t offset);
        uint64_t(*write)(inode_t inode, void* ptr, uint64_t length, uint64_t offset);
        uint64_t(*stat)(inode_t inode, stat_t* stat);
        uint64_t(*isatty)(inode_t inode);
        dirent_t* (*readdir)(inode_t inode, uint64_t num);
        inode_t(*finddir)(inode_t inode, const char* name);
    } vfs_driver_t;

    typedef struct vfs_node
    {
        char name[VFS_NAME_MAX_SIZE];
        inode_t parent;
        inode_t child;
        inode_t older;
        inode_t younger;
        uint64_t type;
        vfs_driver_t* driver;
        int data;
    } vfs_node_t;
}

namespace CommanderKernael {
	namespace Drives {
		namespace FileSystems {
            namespace EXT2
            {

            }
		}
	}
}

#endif //COMMANDERKERNAEL_DRIVES_FILESYSTEMS_EXT2_HPP