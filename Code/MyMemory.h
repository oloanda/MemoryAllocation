#pragma once

#undef APIENTRY
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <Windows.h>
#include <memoryapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#ifndef DEFAULT_ALIGNMENT
#define DEFAULT_ALIGNMENT (2 * sizeof(void *))
#endif

#define KyloByte 1024LL
#define MegaByte 1024LL * KyloByte
#define GigaByte 1024LL * MegaByte
#define TeraByte 1024LL * GigaByte

struct Memory
{
    unsigned char* buffer;
    size_t totalSize;
    size_t prevOffset;
    size_t currOffset;
};

struct TempMemory
{
    Memory* memory;
    size_t prevOffset;
    size_t currOffset;
};

struct FileResource
{
    FILE* file;
    char* path;
    size_t pathSize;

    void* data;
    size_t size;
};

bool IsPowerOfTwo(uintptr_t x);
uintptr_t AlignForward(uintptr_t ptr, size_t align);
void* ArenaAllocAlign(Memory* memory, size_t size, size_t align);
void* MyMalloc(Memory* memory, size_t size);
void* ArenaResizeAlign(Memory* memory, void* oldMemory, size_t oldSize, size_t newSize, size_t align);
void* ResizeMemory(Memory* memory, void* oldMemory, size_t oldSize, size_t newSize);
void InitMemory(Memory* memory, size_t size);
void FreeAllMemory(Memory* memory);
TempMemory TempMemoryBegin(Memory* memory);
void TempMemoryEnd(TempMemory temp);
void InitFileResource(FileResource* File ,const char* fileName, Memory* memory);
void LoadFile(FileResource* dest,Memory* memory);

#ifdef MEMORY_IMPLEMENTATION

static bool IsPowerOfTwo(uintptr_t x)
{
    return (x & (x - 1)) == 0;
}

static uintptr_t AlignForward(uintptr_t ptr, size_t align)
{
    uintptr_t p, a, module;

    assert(IsPowerOfTwo(align));

    p = ptr;
    a = (uintptr_t)align;

    module = p & (a - 1);

    if(module != 0)
    {
        p += a - module;
    }
    return p;
}

static void* ArenaAllocAlign(Memory* memory, size_t size, size_t align)
{
    uintptr_t currPtr = (uintptr_t)memory->buffer + (uintptr_t)memory->currOffset;
    uintptr_t offset = AlignForward(currPtr, align);
    offset -= (uintptr_t)memory->buffer;

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

static void* MyMalloc(Memory* memory, size_t size)
{
    return ArenaAllocAlign(memory, size, DEFAULT_ALIGNMENT);
}

static void* ArenaResizeAlign(Memory* memory, void* oldMemory, size_t oldSize, size_t newSize, size_t align)
{
    unsigned char* oldMem = (unsigned char*)oldMemory;

    assert(IsPowerOfTwo(align));

    if(oldMem == NULL || oldSize == 0)
    {
        return ArenaAllocAlign(memory, newSize, align);
    }
    else if(memory->buffer <= oldMem && oldMem < memory->buffer + memory->totalSize)
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
            size_t copySize = oldSize < newSize ? oldSize : newSize;
            memmove(newMemory, oldMemory, copySize);
            return newMemory;
        }
    }
    else
    {
        assert(0 && "Memory is out of bounds !!!");
        return NULL;
    }
}

static void* ResizeMemory(Memory* memory, void* oldMemory, size_t oldSize, size_t newSize)
{
    return ArenaResizeAlign(memory, oldMemory,oldSize, newSize, DEFAULT_ALIGNMENT);
}

static void InitMemory(Memory* memory, size_t size)
{
    memory->buffer = (unsigned char*)VirtualAlloc(0, size,MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    memory->totalSize = size;
    memory->prevOffset = 0;
    memory->currOffset = 0;
}

static void FreeAllMemory(Memory* memory)
{
    VirtualFree(memory->buffer, 0, MEM_RELEASE);
    memory->currOffset = 0;
    memory->prevOffset = 0;
}

static TempMemory TempMemoryBegin(Memory* memory)
{
    TempMemory temp;
    temp.memory = memory;
    temp.prevOffset = memory->prevOffset;
    temp.currOffset = memory->currOffset;
    return temp;
}

static void TempMemoryEnd(TempMemory temp)
{
    temp.memory->prevOffset = temp.prevOffset;
    temp.memory->currOffset = temp.currOffset;
}


static void InitFileResource(FileResource* File ,const char* fileName, Memory* memory)
{
    File->file = nullptr;
    File->pathSize = strlen(fileName) + 255 + 1;
    File->path = (char *)MyMalloc(memory, File->pathSize);
    strcpy_s(File->path, File->pathSize, fileName);
    File->data = nullptr;
    File->size = 0;
}

static void LoadFile(FileResource* dest,Memory* memory)
{
    if(dest->file != nullptr)
        fclose(dest->file);
    fopen_s(&dest->file, dest->path, "rb");
    if(dest->file != nullptr)
    {
        fseek(dest->file, 0, SEEK_END);
        dest->size = ftell(dest->file);
        fseek(dest->file, 0, SEEK_SET);
        dest->data = MyMalloc(memory,dest->size);

        char *destination = (char *)dest->data;
        size_t totalBytesRead = 0;
        size_t bytesRead = 0;
        while(totalBytesRead < dest->size)
        {
            bytesRead = fread(&destination[totalBytesRead], 1, dest->size - totalBytesRead, dest->file);
            totalBytesRead +=bytesRead;
        }
        destination[totalBytesRead] = '\0';
    }
}

#undef MEMORY_IMPLEMENTATION
#endif
