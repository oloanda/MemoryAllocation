#include "Texture.h"
#include "Matrices.h"
#include "glad/glad.h"


struct Sprite
{
    Texture diffuseMap = {};
    mat4 modelMatrix;
    vec3 spriteColor = vec3(1, 1, 1);
    unsigned int VAO = 0;
    unsigned int VBO = 0;
    unsigned int EBO = 0;

};

void GenerateSpriteBuffer(Sprite& sprite)
{
	float vertices[] = {
		-0.5f, 0.5f, 0.f, 0.f, 0.f, 0.0f, 0.f, 0.f,
		0.5f, 0.5f, 0.f, 0.f, 0.f, 0.0f, 1.f, 0.f, 
		0.5f,-0.5f, 0.f, 0.f, 0.f, 0.0f, 1.f, 1.f, 
		-0.5f,-0.5f, 0.f, 0.f, 0.f, 0.0f, 0.f, 1.f  
	};

	unsigned int indices[] = {
		0, 1, 2,
		2, 3, 0
	};

    glGenVertexArrays(1, &sprite.VAO);
    glBindVertexArray(sprite.VAO);

    glGenBuffers(1, &sprite.VBO);
    glBindBuffer(GL_ARRAY_BUFFER, sprite.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &sprite.EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sprite.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);


}


Sprite CreateSprite(const char* textureFile)
{
    Sprite sprite = {};
    sprite.modelMatrix = mat4();
    sprite.diffuseMap = {};
    sprite.spriteColor = vec3(1,1,1);
    if(textureFile)
    {
        sprite.diffuseMap = Texture(textureFile);
    }

    GenerateSpriteBuffer(sprite);
    return sprite;
}