#pragma once
#pragma warning(disable : 4312)

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "glad/glad.h"

class Texture
{
public:
    unsigned int index = 0;
    int width = 0;
    int height = 0;
    int nbComponents = 0;

    Texture() {}
    Texture(const char* str)
    {
        glGenTextures(1, &index);

        stbi_set_flip_vertically_on_load(true);
        unsigned char* data = stbi_load(str, &width, &height, &nbComponents, 0);
        if(data)
        {
            GLenum format;
            if(nbComponents == 1)
                format = GL_RED;
            else if(nbComponents == 3)
                format = GL_RGB;
            else if(nbComponents == 4)
                format = GL_RGBA;
            
            glBindTexture(GL_TEXTURE_2D, index);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            //glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glBindTexture(GL_TEXTURE_2D, 0);


            stbi_image_free(data);
        }
        else
        {
            printf("Texture failed to load at path: %s\n", str);
            stbi_image_free(data);
            return;
        }        
    }
};