#include <stdio.h>

#define MEMORY_IMPLEMENTATION
#include "MyMemory.h"

int main(int argc, char** argv)
{
    Memory memory = {};
    InitMemory(&memory, 4LL * GigaByte);
    char* str;
    str = (char *)MyMalloc(&memory, 12);

    memmove(str, "JE T'AIME", 9);

    printf("%s\n", str);

    ResizeMemory(&memory,str, 12, 20);
    memmove(str + 9, " TROP !!!", 8);
    
    printf("%s\n", str);


    FreeAllMemory(&memory);
    return 0;
}