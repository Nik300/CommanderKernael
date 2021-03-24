#ifndef __LIBC_SYSTEM_COLLECTIONS_GENERIC_HPP
#define __LIBC_SYSTEM_COLLECTIONS_GENERIC_HPP

#define NULL 0

#include <Headers/environment.hpp>
#include <Headers/drivers/driver.hpp>
#include <Headers/stdio.hpp>
#include <Headers/types.hpp>
#include <Headers/core.hpp>
#include <Headers/ports.hpp>
#include <Headers/util.hpp>
#include <libc/exceptions.hpp>

using namespace CommanderKernael::Drivers;
using namespace CommanderKernael::Drivers::stdio;
using namespace CommanderKernael::utilities::ASCII_UTIL;
using namespace CommanderKernael::utilities::memory;
using namespace assembly::ports;
using namespace assembly::types;
using namespace CommanderKernael::core;

using namespace System::Exceptions;

namespace System {
	namespace Collections {
		namespace Generic {
			template<typename T> class ArrayElement {
			private:
				T* element;
			public:
				ArrayElement(T* element){
					this->element = element;
				}
				operator T(){
					return *element;
				}
				operator T*(){
					return element;
				}
				void operator=(T element){
					*this->element = element;
				}
			};
			template<typename T> class List {
			private:
				ArrayElement<T>* Contents[1024];
				void CheckPos(int pos){
					if (pos >= Count){
						ExceptionHandler::Throw(IndexOutOfRangeException());
					}
					else if (pos < 0){
						ExceptionHandler::Throw(IndexOutOfRangeException());
					}
				}
			public:
				int Count;
				List(){
					//Contents[0] = NULL;
				}
				void Add(T* element) {
					if (Count >= 1024) return;
					ArrayElement<T>* i = new ArrayElement<T>(element);
					Contents[Count++] = i;
					Contents[Count] = NULL;
				}
				void Add(T element) {
					if (Count >= 1024) return;
					T* ptr = (T*)core::memory::allocateMemory(sizeof(T), (void*)&element);
					ArrayElement<T>* i = new ArrayElement<T>(ptr);
					Contents[Count++] = i;
					Contents[Count] = NULL;
				}
				void AddRange(T* list, int size){
					for (int i = 0; i < size; i++)
						Add(list[i]);
				}
				void Clear() {
					Contents[0] = NULL;
					Count = 0;
				}
				void Insert(int pos, T element) {
					CheckPos(pos);
					T* ptr = (T*)core::memory::allocateMemory(sizeof(T)+3, (void*)&element);
					ArrayElement<T>* i = new ArrayElement<T>(ptr);
					Contents[Count++] = i;
					Contents[Count] = NULL;
				}
				void Insert(int pos, T* element) {
					CheckPos(pos);
					ArrayElement<T>* i = new ArrayElement<T>(element);
					Contents[Count++] = i;
					Contents[Count] = NULL;
				}
				void RemoveAt(int pos){
					CheckPos(pos);
                    for (int i = 1; i < Count; i++)
                        Contents[pos + (i - 1)] = Contents[pos + i];
					Contents[Count - 1] = NULL;
					Contents[Count] = 0;
					Count--;
				}
				ArrayElement<T> operator[](int pos) {
					CheckPos(pos);
					return *Contents[pos];
				}
				T* ToArray(){
					if (Count <= 0) return NULL;
					T* result = (T*)core::memory::allocateMemory(sizeof(T)*Count+20);
					for(int i = 0; Contents[i] != NULL; i++)
						result[i] = *Contents[i];
					return result;
				}
			};
		}
	}
}

#endif //__LIBC_SYSTEM_COLLECTIONS_GENERIC_HPP