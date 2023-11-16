#pragma once
#include "ResourceManager.h"
#include "glad/glad.h"
#include <stdio.h>

class Shader
{
public:
    unsigned int index = 0;
    char* sourceData;

    Shader(const char* fileName, GLenum shaderType, Memory* memory)
    {
        FileResource resource(fileName, memory);
        LoadFile(resource, memory);


        sourceData = (char *)resource.data;
        index = glCreateShader(shaderType);
        glShaderSource(index, 1, &sourceData, NULL);
        glCompileShader(index);


        int success;
        char infoLog[512] = {0};
        glGetShaderiv(index, GL_COMPILE_STATUS, &success);

        if(!success)
        {
            glGetShaderInfoLog(index, 512, NULL, infoLog);
            printf("ERROR::SHADER:COMPILATION_FAILED :%s\n", infoLog);
        }
    }
};