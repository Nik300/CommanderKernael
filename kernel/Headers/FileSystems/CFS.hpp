#ifndef COMMANDERKERNAEL_DRIVES_FILESYSTEMS_CFS_HPP
#define COMMANDERKERNAEL_DRIVES_FILESYSTEMS_CFS_HPP

#include <Headers/types.hpp>
#include <Headers/util.hpp>
#include <Headers/drivers/ata.hpp>
#include <Headers/stdio.hpp>
#include <Headers/FileSystems/VFS.hpp>

#include <libc/console.hpp>
#include <libc/exceptions.hpp>
#include <libc/random.hpp>

using namespace assembly::types;
using namespace CommanderKernael::utilities::memory;
using namespace CommanderKernael::utilities::ASCII_UTIL;
using namespace CommanderKernael::Drivers;
using namespace CommanderKernael::Drivers::stdio;

using namespace System;
using namespace System::Random;
using namespace System::Exceptions;

namespace CommanderKernael {
	namespace Drives {
		namespace FileSystems {
			namespace CFS {
				uint8_t magic = 0x5F;
				enum identifiers: uint8_t{
					SectorID = 0x0A,
					Table = 0x0B,
					Part = 0x0C,
					Dir = 0x0D,
					FFOC = 0x0E,
					File = 0x0F,
				};
				namespace Entry{
					struct CommonID{
						uint8_t magic;
						uint8_t id;
					}__attribute__((packed));
					struct Sector{
						CommonID identifier;
						uint8_t position;
						uint8_t Data[509];
					}__attribute__((packed));
					struct Content{
						uint32_t Size;
						uint64_t sector;
						uint32_t pos;
					}__attribute__((packed));
					struct NextTablePartition{
						bool isNull;
						uint64_t sector;
						uint32_t pos;
					}__attribute__((packed));
					struct NextChainElement{
						bool isNull;
						uint64_t sector;
						uint32_t pos;
					}__attribute__((packed));

					struct Table{
						CommonID identifier;
						uint32_t partitions;	  //number of partitions in the current table
						NextTablePartition First; //first partition of the current table
						Content TableName;	   	  //Name of the current table
					}__attribute__((packed));
					struct Directory{
						uint32_t Flags;
						Content name;		  	   //Name of the current directory
						NextChainElement fisrtfile;//First file of this directory's chain
						NextChainElement nextDir;  //Next directory
						NextChainElement subDir;   //First sub directory of this directory
					}__attribute__((packed));
					struct File{
						uint32_t Flags;
						Content fileName;
						Content fileContent;
					}__attribute__((packed));
					struct Partition{
						uint32_t position;
						char PartitionID[10]; //Random value that determines the unique id of this partition
						uint32_t PartitionNum;//Index of the current partition
						uint32_t Size;		  //Size, in sectors, of the current partition
						uint32_t Flags;	  	  //Flags of the current partition
						Content PartitionName;//Name of the current partition
						NextChainElement rootDir;
						uint32_t posInData;
						NextTablePartition next;
					}__attribute__((packed));
					struct DataSpectrum{
						uint32_t init;
					}__attribute__((packed));
				}
				class FileSystem: public VFS{
				private:
					void LoadSector(int n){
						console->printf("");
						console->printf("");
						sectorNum = n;
						AdvancedTechnologyAttachement::Read(n, 1, (void*)currentSector);
					}
					void SaveSector(int n){
						console->printf("");
						console->printf("");
						AdvancedTechnologyAttachement::Write(n, 1, (void*)currentSector);
					}
					void LoadPartition(int Sector, int pos){
						if (currentPartition == NULL)
							currentPartition = new Entry::Partition;
						if (Sector <= 0)
							ExceptionHandler::Throw(Exceptions::SectorReservedException());
						LoadSector(Sector);
						ReadFromSector(pos, sizeof(Entry::Partition), (void*)currentPartition);
						spectrum.init = currentPartition->posInData;
					}
					void SavePartition(){
						if (currentPartition == NULL)
							ExceptionHandler::Throw(ArgumentNullException("currentPartition"));
						WriteToSector(currentPartition->position, sizeof(Entry::Partition), (void*)currentPartition);
					}
					void ReadFromSector(int from, int size, void* result){
						//if (from >= 509 || size + from >= 509)
							//ExceptionHandler::Throw(IndexOutOfRangeException());
						uint8_t* dat = (uint8_t*)result;
						int initSector = sectorNum;
						for (int i = from, s = 0; s < size; i++,s++){
							if (i >= 509){
								LoadSector(sectorNum+1);
								i = 0;
							}
							dat[s] = currentSector->Data[i];
						}
						LoadSector(initSector);
					}
					void ReadFromSector(int size, void* result){
						ReadFromSector(currentSector->position, size, result);
					}
					void WriteToSector(int position, int size, void* data){
						//if (position >= 509 || size + position >= 509)
						//	ExceptionHandler::Throw(IndexOutOfRangeException());
						debug::send_string(SERIAL_PORT_A, "[CFS] Init sector writing\n");
						//debug::send_string(SERIAL_PORT_A, intToASCII(position));
						uint8_t* dat = (uint8_t*)data;
						int i, s;
						int initSector = sectorNum;
						for (i = position, s = 0; s < size; i++,s++){
							if (i >= 509){
								debug::send_string(SERIAL_PORT_A, "[CFS] Swapping sector\n");
								currentSector->position = -1;
								SaveSector(sectorNum);
								LoadSector(sectorNum + 1);
								currentSector->position = 0;
								i = 0;
								debug::send_string(SERIAL_PORT_A, "[CFS] Sector swapped\n");
							}
							currentSector->Data[i] = dat[s];
						}
						if (i > currentSector->position)
							currentSector->position = i;
						SaveSector(sectorNum);
						LoadSector(initSector);
					}
					void WriteToSector(int size, void* data){
						while (currentSector->position == -1)
							LoadSector(sectorNum + 1);
						WriteToSector(currentSector->position, size, data);
					}
					void InitTable(){
						if (currentTable == NULL)
							currentTable = new Entry::Table;
						ReadFromSector(0, sizeof(Entry::Table), (void*)currentTable);
					}
					void SaveTable(){
						if (currentTable == NULL)
							ExceptionHandler::Throw(ArgumentNullException("currentTable"));
						WriteToSector(0, sizeof(Entry::Table), (void*)currentTable);
					}
					void LoadDataSpectrum(){
						if (data == NULL)
							data = (uint8_t*)core::memory::allocateMemory(100);

						ReadFromSector(spectrum.init + currentPartition->PartitionName.pos + currentPartition->PartitionName.Size, 100, (void*)data);
					}
					void SaveDataSpectrum(){
						if (data == NULL)
							ExceptionHandler::Throw(ArgumentNullException("data"));
						
						WriteToSector(spectrum.init + currentPartition->PartitionName.pos + currentPartition->PartitionName.Size, 100, (void*)data);
						SaveSector(sectorNum);
					}
					void SwitchSpectrum(){
						if (data == NULL)
						{
							LoadDataSpectrum();
							return;
						}
						SaveDataSpectrum();
						if (spectrum.init == currentPartition->posInData) {
							currentPartition->posInData += 100;
						}
						spectrum.init += 100;
						LoadDataSpectrum();
					}
					bool GetNextDir(Entry::NextChainElement source, Entry::Directory* result){
						if (source.isNull)
							return false;
						int initSector = sectorNum;
						LoadSector(source.sector);
						ReadFromSector(source.pos, sizeof(Entry::Directory), (void*)result);
						LoadSector(initSector);
						return true;
					}
					string ReadName(Entry::Content name){
						int initSector = sectorNum;
						LoadSector(name.sector);
						String res = (String)core::memory::allocateMemory(name.Size);
						ReadFromSector(name.pos, name.Size, (void*)res);
						return string(res);
					}
					Entry::Directory GetDir(string path){
						if (currentPartition == NULL)
							throw(Exceptions::PartitionNotSelected());
						Entry::Directory next;
						Entry::Directory result;
						GetNextDir(currentPartition->rootDir, &next);
						int len;
						string* res = path.split('/', &len);
						for (int si = 0; si < len;){
							if (ReadName(next.name) == res[si] && si == len - 1){
								result = next;
								break;
							}
							else if (ReadName(next.name) == res[si]){
								si++;
								if (next.subDir.isNull) throw(Exceptions::DirectoryNotFoundException(res[si]));
								GetNextDir(next.subDir, &next);
								continue;
							}
							else if (next.nextDir.isNull){
								ExceptionHandler::Throw(Exceptions::DirectoryNotFoundException(res[si]));
							}
							else {
								GetNextDir(next.nextDir, &next);
								continue;
							}
						}
						return result;
					}
					Entry::Directory GetDir(string* res, int len){
						if (currentPartition == NULL)
							throw(Exceptions::PartitionNotSelected());
						debug::send_string(SERIAL_PORT_A, "[CFS] Getting dir\n");
						Entry::Directory next;
						Entry::Directory result;
						GetNextDir(currentPartition->rootDir, &next);
						for (int si = 0; si < len;){
							if (ReadName(next.name) == res[si] && si == len - 1){
								result = next;
								break;
							}
							else if (ReadName(next.name) == res[si]){
								si++;
								if (next.subDir.isNull) ExceptionHandler::Throw(Exceptions::DirectoryNotFoundException(res[si]));
								GetNextDir(next.subDir, &next);
								continue;
							}
							else if (next.nextDir.isNull){
								ExceptionHandler::Throw(Exceptions::DirectoryNotFoundException(res[si]));
							}
							else {
								GetNextDir(next.nextDir, &next);
								continue;
							}
						}
						debug::send_string(SERIAL_PORT_A, "[CFS] Got dir\n");
						return result;
					}
					void SetDir(string path, Entry::Directory dir){
						if (currentPartition == NULL)
							throw(Exceptions::PartitionNotSelected());
						int pos;
						int sector;
						int toPos = currentSector->position;
						int toSector = sectorNum;
						WriteToSector(sizeof(Entry::Directory), (void*)&dir);
						Entry::Directory next;
						Entry::Directory result;
						Entry::NextChainElement e;
						GetNextDir(currentPartition->rootDir, &next);
						e.pos = currentPartition->rootDir.pos;
						e.sector = currentPartition->rootDir.sector;
						pos = currentPartition->rootDir.pos;
						sector = currentPartition->rootDir.sector;
						int len;
						string* res = path.split('/', &len);
						len--;
						debug::send_string(SERIAL_PORT_A, string("starting at sector ") + intToASCII(e.sector) + " at position " + intToASCII(e.pos) + "\n");
						for (int si = 0; si < len;){
							if (ReadName(next.name) == res[si] && si == len - 1){
								if (!next.subDir.isNull){
									while(!next.nextDir.isNull)
									{
										pos = next.nextDir.pos;
										sector = next.nextDir.sector;
										e = next.nextDir;
										GetNextDir(next.nextDir, &next);
									}
									next.nextDir = CreateChainElement(toPos, toSector);
									next.subDir.isNull = false;
								}
								else {
									next.subDir = CreateChainElement(toPos, toSector);
									next.subDir.isNull = false;
								}
								debug::send_string(SERIAL_PORT_A, string("saving changes to sector ") + intToASCII(e.sector) + " at position " + intToASCII(e.pos) + "\n");
								LoadSector(e.sector);
								WriteToSector(e.pos, sizeof(Entry::Directory), (void*)&next);
								SaveSector(sectorNum);
								break;
							}
							else if (ReadName(next.name) == res[si]){
								debug::send_string(SERIAL_PORT_A, "[CFS] Found parent\n");
								si++;
								if (next.subDir.isNull) throw(Exceptions::DirectoryNotFoundException(res[si]));
								pos = next.subDir.pos;
								sector = next.subDir.sector;
								e = next.subDir;
								GetNextDir(next.subDir, &next);
								continue;
							}
							else if (next.nextDir.isNull){
								throw(Exceptions::DirectoryNotFoundException(res[si]));
							}
							else {
								pos = next.nextDir.pos;
								sector = next.nextDir.sector;
								e = next.nextDir;
								GetNextDir(next.nextDir, &next);
								continue;
							}
						}
						debug::send_string(SERIAL_PORT_A, "[CFS] Wrote directory\n");
					}
					bool GetPartition(int partNum){
						Entry::NextTablePartition next = currentTable->First;
						LoadPartition(currentTable->First.sector, currentTable->First.pos);
						for (int i = 0; i < currentTable->partitions; i++){
							if (currentPartition->PartitionNum == partNum){
								return true;
								break;
							}
							else {
								if (currentPartition->next.isNull && currentPartition->next.pos == -1 && currentPartition->next.sector == -1){
									debug::send_string(SERIAL_PORT_A, "Not found\n");
									return false;
								}
								LoadPartition(currentPartition->next.sector, currentPartition->next.pos);
							}
						}
						return false;
					}
					Entry::NextChainElement CreateChainElement(int pos, int sector){
						Entry::NextChainElement res;
						res.isNull = false;
						res.pos = pos;
						res.sector = sector;
						return res;
					}
				public:
					Entry::Sector* currentSector = new Entry::Sector;
					Entry::Table* currentTable = NULL;
					Entry::Partition* currentPartition = NULL;
					Entry::DataSpectrum spectrum;
					uint8_t* data = NULL;
					uint64_t sectorNum;

					FileSystem() {
						spectrum.init = 0;
					}

					void Initialize(int sectors, bool quick) override{
						debug::send_string(SERIAL_PORT_A, "[CFS] initializing disk to CFS format\n");
						for (int i = 0; i < sectors; i++){
							LoadSector(i);
							currentSector->identifier.magic = magic;
							currentSector->identifier.id = SectorID;
							currentSector->position = 0;
							SaveSector(i);
							if (!quick)
								for (int x = 0; x < 510; x++)
									currentSector->Data[x] = 0;
							SaveSector(i);
						}
					}
					void Initialize(int startSector, int sectors, bool quick) override{
						debug::send_string(SERIAL_PORT_A, "[CFS] initializing disk to CFS format\n");
						for (int i = startSector; i < sectors; i++){
							LoadSector(i);
							currentSector->identifier.magic = magic;
							currentSector->identifier.id = SectorID;
							currentSector->position = 0;
							SaveSector(i);
							if (!quick)
								for (int x = 0; x < 510; x++)
									currentSector->Data[x] = 0;
							SaveSector(i);
						}
					}
					void CreateTable(string TableName) override{
						LoadSector(0);
						InitTable();
						currentTable->identifier.magic = magic;
						currentTable->identifier.id = Table;
						currentTable->partitions = 0;
						currentTable->TableName.pos = sizeof(Entry::Table);
						currentTable->TableName.sector = sectorNum;
						currentTable->TableName.Size = TableName.lenght();
						currentTable->First.isNull = true;
						currentTable->First.pos = -1;
						currentTable->First.sector = -1;
						currentSector->position = 0;
						partitions = 0;
						SaveTable();
						WriteToSector(TableName.lenght(), (void*)TableName._iterator);
						SaveSector(0);
						debug::send_string(SERIAL_PORT_A, "[CFS] Table created\n");
					}
					void CreatePartition(string name, int startSector, int sizeInKB, string root, int posInSector = 0, bool hidden = false, bool perms = true, char type = 0, bool quick = true) override{
						if (startSector <= 0)
							ExceptionHandler::Throw(Exceptions::SectorReservedException());
						if (currentTable == NULL){
							LoadSector(0);
							InitTable();
						}
						int sectorCount = (sizeInKB*1024)/512;
						Initialize(startSector, sectorCount, quick);
						LoadPartition(startSector, posInSector);
						Rand rand;
						string str = rand.RandString(9);
						for (int i = 0; i < 9; i++)
							currentPartition->PartitionID[i] = str._iterator[i];
						currentPartition->PartitionID[9] = 0;
						currentPartition->Flags = (0 << 32) | (hidden << 16) | (perms << 8) | type;
						currentPartition->next.isNull = true;
						currentPartition->next.pos = -1;
						currentPartition->next.sector = -1;
						currentPartition->PartitionNum = partitions + 1;
						currentPartition->position = posInSector;
						currentPartition->Size = sectorCount;
						currentPartition->PartitionName.pos = posInSector + sizeof(Entry::Partition);
						currentPartition->PartitionName.Size = name.lenght();
						currentPartition->PartitionName.sector = sectorNum;
						currentPartition->posInData = 0;
						currentPartition->rootDir.isNull = false;
						currentPartition->rootDir.pos = posInSector + sizeof(Entry::Partition) + name.lenght();
						currentPartition->rootDir.sector = sectorNum;
						core::debug::send_string(SERIAL_PORT_A, string("[CFS] Root dir for partition ") + intToASCII(currentPartition->PartitionNum) + " is at sector " + intToASCII(currentPartition->rootDir.sector) + " at postion " + intToASCII(currentPartition->rootDir.pos) + "\n");
						SavePartition();
						WriteToSector(posInSector + sizeof(Entry::Partition), name.lenght(), (void*)name._iterator);
						Entry::Directory rootDir;
						rootDir.name.pos = currentPartition->rootDir.pos + sizeof(Entry::Directory);
						rootDir.name.Size = root.lenght();
						rootDir.name.sector = sectorNum;
						rootDir.fisrtfile.isNull = true;
						rootDir.nextDir.isNull = true;
						rootDir.subDir.isNull = true;
						rootDir.subDir.pos = -1;
						rootDir.subDir.sector = -1;
						rootDir.Flags = 0;
						WriteToSector(currentPartition->rootDir.pos, sizeof(Entry::Directory), (void*)&rootDir);
						WriteToSector(rootDir.name.pos, root.lenght(), (void*)root._iterator);
						SaveSector(sectorNum);
						LoadSector(0);
						currentTable->partitions++;
						partitions++;
						if (currentTable->First.isNull){
							currentTable->First.sector = startSector;
							currentTable->First.pos = posInSector;
							currentTable->First.isNull = false;
						}
						else{
							Entry::NextTablePartition np;
							np.isNull = false;
							np.pos = posInSector;
							np.sector = startSector;
							LoadPartition(currentTable->First.sector, currentTable->First.pos);
							for (int i = 0; i < partitions; i++){
								if (currentPartition->next.isNull){
									currentPartition->next.sector = startSector;
									currentPartition->next.pos = posInSector;
									currentPartition->next.isNull = false;
									SavePartition();
									SaveSector(sectorNum);
									break;
								}
								else LoadPartition(currentPartition->next.sector, currentPartition->next.pos);
							}
							LoadSector(0);
							SaveTable();
							SaveSector(0);
						}
						debug::send_string(SERIAL_PORT_A, "[CFS] Partition created\n");
					}
					bool PartIsRW(int partNum) override{
						console->printf("");
						if (!GetPartition(partNum))
							ExceptionHandler::Throw(Exception("Partition not found"));
						return (bool)((currentPartition->Flags & 0x0000FF00)>>8);
					}
					bool PartIsHidden(int partNum) override{
						console->printf("");
						if (!GetPartition(partNum))
							ExceptionHandler::Throw(Exception("Partition not found"));
						return (bool)((currentPartition->Flags & 0x00FF0000)>>16);
					}
					string PartName(int partNum) override{
						console->printf("");
						if (!GetPartition(partNum))
							ExceptionHandler::Throw(Exception("Partition not found"));
						return ReadName(currentPartition->PartitionName);
					}
					string GetPartID(int partNum) override{
						console->printf("");
						if (!GetPartition(partNum))
							ExceptionHandler::Throw(Exception("Partition not found"));
						return currentPartition->PartitionID;
					}
					void WriteToPartition(int position, int partNum, void* data, int size) override{
						//console->printf("");
						if (!GetPartition(partNum))
							ExceptionHandler::Throw(Exception("Partition not found"));
						uint8_t* dat = (uint8_t*)data;

						spectrum.init = position;
						LoadDataSpectrum();

						for (int s = 0; s < size; s++){
							if (s >= 100)
								SwitchSpectrum();
							this->data[s] = dat[s];
						}
						SavePartition();
						SaveDataSpectrum();
						SaveSector(sectorNum);
					}
					void WriteToPartition(int partNum, void* data, int size) override{
						console->printf("");
						if (!GetPartition(partNum))
							ExceptionHandler::Throw(Exception("Partition not found"));
						uint8_t* dat = (uint8_t*)data;

						spectrum.init = currentPartition->position;

						LoadDataSpectrum();

						for (int s = 0; s < size; s++){
							if (s >= 100){
								debug::send_string(SERIAL_PORT_A, "[CFS] Switching data spectrum\n");
								SwitchSpectrum();
								debug::send_string(SERIAL_PORT_A, "[CFS] Switched\n");
								s = 0;
								size -= 100;
							}
							this->data[s] = dat[s];
						}
						currentPartition->position = spectrum.init;
						SavePartition();
						SaveDataSpectrum();
						SaveSector(sectorNum);
					}
					void CreateDir(int partNum, string path) override {
						console->printf("");
						if (!GetPartition(partNum))
							ExceptionHandler::Throw(Exception("Partition not found"));
						int len;
						string* r = path.split('/', &len);
						Entry::Directory nDir;
						nDir.fisrtfile.isNull = true;
						nDir.Flags = 0;
						nDir.name.pos = currentSector->position;
						WriteToSector(r[len-1].lenght(), (void*)r[len-1]._iterator);
						SaveSector(sectorNum);
						nDir.name.sector = sectorNum;
						nDir.name.Size = r[len-1].lenght();
						nDir.nextDir.isNull = true;
						nDir.subDir.isNull = true;
						nDir.nextDir.pos = -1;
						nDir.nextDir.sector = -1;
						nDir.subDir.pos = -1;
						nDir.subDir.sector = -1;
						SetDir(path, nDir);
					}
					string GetDirName(int partNum, string path) override {
						console->printf("");
						if (!GetPartition(partNum))
							ExceptionHandler::Throw(Exception("Partition not found"));
						return ReadName(GetDir(path).name);
					}
					string GetRootDirName(int partNum) override {
						console->printf("");
						if (!GetPartition(partNum))
							ExceptionHandler::Throw(Exception("Partition not found"));
						Entry::Directory dir;
						GetNextDir(currentPartition->rootDir, &dir);
						core::debug::send_string(SERIAL_PORT_A, string("[CFS] Root dir for partition ") + intToASCII(partNum) + " is at sector " + intToASCII(currentPartition->rootDir.sector) + " at postion " + intToASCII(currentPartition->rootDir.pos) + "\n");
						return ReadName(dir.name);
					}
				};
			}
		}
	}
}
#endif // COMMANDERKERNAEL_DRIVES_FILESYSTEMS_CFS_HPP
