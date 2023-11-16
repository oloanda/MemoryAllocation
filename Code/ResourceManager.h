#pragma once

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN

#undef APIENTRY
#include <Windows.h>
#include <memoryapi.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>

#include "MyString.h"

#define KiloByte 1024LL
#define MegaByte 1024LL * KiloByte
#define GigaByte 1024LL * MegaByte
#define TeraByte 1024LL * GigaByte

#ifndef DEFAULT_ALIGNMENT
#define DEFAULT_ALIGNMENT ( 2 * sizeof(void *))
#endif


bool IsPowerOfTwo(uintptr_t x)
{
    return (x & (x - 1)) == 0;
}

uintptr_t AlignForward(uintptr_t ptr, size_t align)
{
    assert(IsPowerOfTwo(align));

    uintptr_t p, module ,a;
    p = ptr;
    a = (uintptr_t)align;

    module = p & (a - 1);

    if(module != 0)
    {
        p += a - module;
    }
    return p;
}

struct Memory
{
    unsigned char* buffer;
    size_t totalSize;
    size_t prevOffset;
    size_t currOffset;
    bool isInitialized;
};

Memory InitMemory(size_t size)
{
    Memory memory = {};
    memory.buffer = (unsigned char *)VirtualAlloc(nullptr, size, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    memory.totalSize = size;
    memory.prevOffset = 0;
    memory.currOffset = 0;
    return memory;
}

void FreeAllMemory(Memory* memory)
{
    VirtualFree(memory->buffer, 0, MEM_RELEASE);
    memory->prevOffset = 0;
    memory->currOffset = 0;
}


void* ArenaAllocAlign(Memory* memory, size_t size, size_t align)
{
    uintptr_t currPtr = (uintptr_t)memory->buffer + memory->currOffset;
    uintptr_t offset = AlignForward(currPtr, align);
    offset -=(uintptr_t)memory->buffer;

    if(offset + size <= memory->totalSize)
    {
        void* ptr = &memory->buffer[offset];
        memory->prevOffset = offset;
        memory->currOffset = offset + size;

        memset(ptr, 0, size);
        return ptr;
    }
    return NULL;
}

void* MyMalloc(Memory* memory, size_t size)
{
    return ArenaAllocAlign(memory, size, DEFAULT_ALIGNMENT);
}

void* ArenaResizeAlign(Memory* memory, void* oldMemory, size_t oldSize, size_t newSize, size_t align)
{
    unsigned char* oldMem = (unsigned char *)oldMemory;

    assert(IsPowerOfTwo(align));

    if(oldMem == NULL || oldSize == 0)
    {
        return ArenaAllocAlign(memory, newSize, align);
    }
    else if(memory->buffer <= oldMem && (uintptr_t)oldMem < (uintptr_t)memory->buffer + memory->totalSize)
    {
        if(memory->buffer + memory->prevOffset == oldMem)
        {
            memory->currOffset = memory->prevOffset + newSize;
            if(newSize > oldSize)
            {
                memset(&memory->buffer[memory->currOffset], 0, newSize - oldSize);

            }
            return oldMemory;
        }
        else
        {
            void* newMemory = ArenaAllocAlign(memory, newSize, align);
            size_t copysize = oldSize < newSize ? oldSize : newSize;
            memmove(newMemory, oldMemory, copysize);
            return newMemory;
        }
    }
    else
    {
        assert(0 && "Memory is out of bounds of the buffer in this arena !!\n");
        return NULL;
    }
}

void* ResizeMemory(Memory* memory, void* oldMemory, size_t oldSize, size_t newSize)
{
    return  ArenaResizeAlign(memory, oldMemory, oldSize, newSize, DEFAULT_ALIGNMENT);
}

struct TempMemory
{
    Memory* memory;
    size_t prevOffset;
    size_t currOffset;
};

TempMemory TempMemoryBegin(Memory* memory)
{
    TempMemory temp = {};
    temp.memory = memory;
    temp.prevOffset = memory->prevOffset;
    temp.currOffset = memory->currOffset;
    return temp;
}

void TempMemoryEnd(TempMemory& temp)
{
    temp.memory->prevOffset = temp.prevOffset;
    temp.memory->currOffset = temp.currOffset;
}

struct FileResource
{
    FILE* file;
    MyString path;
    void* data;
    size_t size;

    FileResource(const char* fileName, Memory* memory)
    {
        file = nullptr;
        path.size = strlen(fileName) + 255 + 1;
        path.data = (char *)MyMalloc(memory, path.size);
        strcpy_s(path.data,path.size ,fileName);
        data = nullptr;
        size = 0;
    }
};

void LoadFile(FileResource& resource, Memory* memory)
{
    if(resource.file != nullptr)
        fclose(resource.file); 
    fopen_s(&resource.file, resource.path.data, "rb");
    if(resource.file != nullptr)
    {
        fseek(resource.file, 0, SEEK_END);
        resource.size = ftell(resource.file);
        fseek(resource.file, 0, SEEK_SET);
        resource.data = MyMalloc(memory, resource.size);
        fread(resource.data, resource.size, 1, resource.file);
    }
}