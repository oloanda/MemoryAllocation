#pragma once
#include "ResourceManager.h"
#include "Parser.h"
#include "Matrices.h"
#include "Material.h"
#include "meow_hash_x64_aesni.h"
#include <unordered_map>


struct Vertex
{
    vec3 position = {};
    vec3 normal = {};
    vec2 textureUV = {};
    Vertex(){};
	Vertex(vec3 pos, vec3 n, vec2 tex)
    {position = pos; normal = n; textureUV = tex;}
    struct HashFunction
    {
        size_t operator()(const Vertex& v) const
        {
            meow_u128 Hash = MeowHash(MeowDefaultSeed, sizeof(Vertex), (void *)&v);
            long long unsigned Hash64 = MeowU64From(Hash, 0);

            return (Hash64);
        }
    };
};

inline bool operator==(const Vertex &a, const Vertex &b)
{
    if (a.position.x == b.position.x && a.position.y == b.position.y &&
        a.position.z == b.position.z && a.normal.x == b.normal.x && a.normal.y == b.normal.y &&
        a.normal.z == b.normal.z && a.textureUV.x == b.textureUV.x && a.textureUV.y == b.textureUV.y)
        return (true);
    return (false);
}

struct Mesh
{
    Vertex *vertices = nullptr;
    unsigned int *indices = nullptr;
    unsigned int vertexCount = 0;
    unsigned int indexCount = 0;
    Material  material = {};
    unsigned int VAO = 0;
    unsigned int VBO = 0;
    unsigned int EBO = 0;
};

struct Model
{
    Mesh* meshes = nullptr;
    size_t meshCount = 0;
    mat4 modelMatrix;
};

void GenerateBuffers(Mesh* meshes, size_t meshCount)
{
    for(int i = 0; i < meshCount; i++)
    {
        glGenVertexArrays(1, &meshes[i].VAO);
        glBindVertexArray(meshes[i].VAO);

        glGenBuffers(1, &meshes[i].VBO);

        glBindBuffer(GL_ARRAY_BUFFER, meshes[i].VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * meshes[i].vertexCount, meshes[i].vertices, GL_STATIC_DRAW);


        glGenBuffers(1, &meshes[i].EBO);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshes[i].EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * meshes[i].indexCount, meshes[i].indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0,3, GL_FLOAT, GL_FALSE, sizeof(Vertex),(void *)(offsetof(Vertex, position)));
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1,3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(offsetof(Vertex, normal)));
        glEnableVertexAttribArray(1);

        glVertexAttribPointer(2,2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)(offsetof(Vertex, textureUV)));
        glEnableVertexAttribArray(2);

        //glBindVertexArray(0);


    }
}




#if 1

Material ParseMTL(const char* buffer,char* MaterialName ,Memory* memory)
{
    Material material = {};
    material.diffuseMap.index = 0;
    material.normalMap.index = 0;
    material.specularMap.index = 0;
    material.ambient = vec3(0.5f, 0.5f, 0.5f);
    material.diffuse = vec3(0.8f, 0.8f, 0.8f);
    material.specular = vec3(1.0f, 1.0f, 1.0f);
    material.emissive = vec3(1.0f, 1.0f, 1.0f);
    material.opaqueness = 1.0f;
    material.shininess = 25.0f;

    char* current   = (char *)buffer;
    char* endOfLine = (char *)buffer;

    char temp[255] = {};
    int i = 0;
    while(MaterialName[i] != '\n' && MaterialName[i] != ' ' && MaterialName[i] != '\r')
    {
        temp[i] = MaterialName[i];
        i++;
    }
    temp[i] = '\0';
    while(*endOfLine != '\0')
    {
        current = endOfLine;
        endOfLine = GetNextLine(endOfLine);
        while(current != endOfLine && endOfLine > current)
        {
            if(*current == '#')
                current = endOfLine;
            if(strncmp(temp, current, strlen(temp)) == 0 && (current[strlen(temp)] == '\n' || current[strlen(temp)] == '\r'))
            {
                while(strncmp(current, "newmtl ", 7) != 0 && *current != '\0')
                {
                    if(strncmp(current, "norm ", 5) == 0)
                    {
                        i = 0;
                        while(current[i] != '\n' && current[i] != ' ' && current[i] != '\t' && current[i] != '\r')
                            i++;
                        i++;
                        int j = 0;
                        while(current[i] != '\n' && current[i] != ' ' && current[i] != '\t' && current[i] != '\r')
                            temp[j++] = current[i++];
                        temp[j] = '\0';
                        material.normalMap = Texture(&temp[0]);
                        current += 5;
                    }
                    else if(strncmp(current, "map_Kd ", 7) == 0)
                    {
                        i = 0;
                        while(current[i] != '\n' && current[i] != ' ' && current[i] != '\t' && current[i] != '\r')
                            i++;
                        i++;
                        int j = 0;
                        while(current[i] != '\n' && current[i] != ' ' && current[i] != '\t' && current[i] != '\r')
                            temp[j++] = current[i++];
                        temp[j] = '\0';
                        material.diffuseMap = Texture(&temp[0]);
                        current += 7;
                    }
                    else if(strncmp(current, "map_Ks ", 7) == 0)
                    {
                        i = 0;
                        while(current[i] != '\n' && current[i] != ' ' && current[i] != '\t' && current[i] != '\r')
                            i++;
                        i++;
                        int j = 0;
                        while(current[i] != '\n' && current[i] != ' ' && current[i] != '\t' && current[i] != '\r')
                            temp[j++] = current[i++];
                        temp[j] = '\0';
                        material.specularMap = Texture(&temp[0]);
                        current += 7;
                    }
                    else if(strncmp(current, "Kd ", 3) == 0)
                    {
                        current = SkipeWhiteSpace(current);
                        material.diffuse = GetVec3(current);
                    }
                    else if(strncmp(current, "Ks ", 3) == 0)
                    {
                        current = SkipeWhiteSpace(current);
                        material.specular = GetVec3(current);
                    }
                    else if(strncmp(current, "Ke ", 3) == 0)
                    {
                        current = SkipeWhiteSpace(current);
                        material.emissive = GetVec3(current);
                    }
                    else if(strncmp(current, "d ", 2) == 0)
                    {
                        current = SkipeWhiteSpace(current);
                        material.opaqueness = (float)atof(current);
                    }
                    else if(strncmp(current, "Ns ", 3) == 0)
                    {
                        current = SkipeWhiteSpace(current);
                        material.shininess = (float)atof(current);
                    }
                    current++;
                }
                return material;
            }
            current++;
        }
    }
    return material;
}


Model CreateObjVAO(const char* fileName, Memory* memory)
{
    Model model = {};
    model.modelMatrix = mat4();
    Material material = {};

    FILE* objFile = nullptr;
    fopen_s(&objFile, fileName, "rb");
    int totalSize = 0;
    char* buffer = nullptr;
    if(objFile)
    {
        fseek(objFile, 0, SEEK_END); 
        totalSize = ftell(objFile);
        fseek(objFile, 0, SEEK_SET);
        buffer = (char *)MyMalloc(memory, totalSize);
        memset((void *)buffer,0 ,totalSize);
        fread((void *)buffer, totalSize, 1, objFile);
    }
    else
    {
        return model;
    }

    int indexCountPerLine = -1;
    int posCount = 0;
    int normalCount = 0;
    int textureUVCount = 0;
    int materialsCount = 0;
    int indexCount = 0;
    fclose(objFile);

    char* current = buffer;
    char* endOfLine = buffer;
    const char* mtlLib = nullptr;
    bool hasMtlFile = false;
    while(*endOfLine != '\0')
    {
        current = endOfLine;
        endOfLine = GetNextLine(endOfLine);

        while(current != endOfLine && endOfLine > current)
        {
            if(*current == '#')
                current = endOfLine;
            else if(*current == 'v' && current[1] == ' ')
            {
                current = SkipeWhiteSpace(current);
                posCount++;
                current = endOfLine;
            }
            else if(*current == 'v' && current[1] == 't')
            {
                current = SkipeWhiteSpace(current);
                textureUVCount++;
                current = endOfLine;
            }
            else if(*current == 'v' && current[1] == 'n')
            {
                current = SkipeWhiteSpace(current);
                normalCount++;
                current = endOfLine;
            }
            else if(strncmp("mtllib", current, 6) == 0)
            {
                current = SkipeWhiteSpace(current);
                mtlLib = current;
                current = endOfLine;
                hasMtlFile = true;
            }
            else if(( (size_t)endOfLine - (size_t)current) >= 5 && strncmp("usemtl", current, 6) == 0)
            {
                materialsCount++;
                current = endOfLine;
            }
            else if(*current == 'f' && current[1] == ' ')
            {
                if(indexCountPerLine == -1)
                {
                    indexCountPerLine = 0;
                    for(int i = 0; current[i] != '\n'; i++)
                    {
                        if(current[i] == '/')
                            indexCountPerLine++;
                    }
                }
                indexCount += indexCountPerLine / 2;
                current = endOfLine;
                if(indexCountPerLine == 0)
                    indexCount += 3;
            }
            else
                current++;
        }
    }


    int verterIndex = 0;
    int positionIndex = 0;
    int normalIndex = 0;
    int textureUVIndex = 0;
    int meshIndex = 0;
    if(!hasMtlFile)
        materialsCount = 1;
    
    Mesh* meshes = (Mesh *)MyMalloc(memory, materialsCount * sizeof(Mesh));
    TempMemory tempMem = TempMemoryBegin(memory);
    vec3 *position = (vec3 *)MyMalloc(tempMem.memory, posCount * sizeof(vec3));
    vec3 *normal = (vec3 *)MyMalloc(tempMem.memory, normalCount * sizeof(vec3));
    vec2 *textureUV = (vec2 *)MyMalloc(tempMem.memory, textureUVCount * sizeof(vec2));
    Vertex *vertices = (Vertex *)MyMalloc(tempMem.memory, indexCount * sizeof(Vertex));
    unsigned int *indices = (unsigned int *)MyMalloc(tempMem.memory, indexCount * sizeof(unsigned int *));
    char mtlLibFilePath[255] = {};


    meshes[0].material = Material();
    strncpy_s(mtlLibFilePath, "../Assets/",10);

    int i = 0;
    while(mtlLibFilePath[i] != '\0')
        i++;
    int j = 0;
    if(hasMtlFile)
    {
        while(mtlLib[j] != '\n' && mtlLib[j] != '\t' && mtlLib[j] != '\r')
        {
            mtlLibFilePath[i] = mtlLib[j++];
            i++;
        }
        mtlLibFilePath[i] = '\0';
    }

    FileResource mtlLibData(mtlLibFilePath, memory);
    LoadFile(mtlLibData, memory);
    if(mtlLibData.file)
        fclose(mtlLibData.file);
    
    model.meshes = meshes;
    model.meshes[meshIndex].vertices = vertices;
    model.meshes[meshIndex].indices = indices;

    current = buffer;
    endOfLine = buffer;
    model.meshCount = materialsCount;

    while(*endOfLine != '\0')
    {
        current = endOfLine;
        endOfLine = GetNextLine(endOfLine);
        while(current != endOfLine && endOfLine > current)
        {
            if(*current == '#')
            {
                current = endOfLine;
            }
            else if(*current == 'v' && current[1] == ' ')
            {
                current = SkipeWhiteSpace(current);
                position[positionIndex] = GetVec3(current);
                positionIndex++;
                current = endOfLine;
            }
            else if(*current == 'v' && current[1] == 't')
            {
                current = SkipeWhiteSpace(current);
                textureUV[textureUVIndex] = GetVec2(current);
                textureUVIndex++;
                current = endOfLine;
            }
            else if(*current == 'v' && current[1] == 'n')
            {
                current = SkipeWhiteSpace(current);
                normal[normalIndex] = GetVec3(current);
                normalIndex++;
                current = endOfLine;
            }
            else if(strncmp("mtllib", current, 6) == 0)
            {
                current = endOfLine;
            }
            else if(( (size_t)endOfLine - (size_t)current) >= 5 && strncmp("usemtl", current, 6) == 0)
            {
                model.meshes[meshIndex].indices = &indices[verterIndex];
                model.meshes[meshIndex].vertices = &vertices[verterIndex];
                if(meshIndex >= 1)
                {
                    model.meshes[meshIndex - 1].vertexCount = (unsigned int)(&indices[verterIndex] - model.meshes[meshIndex - 1].indices);
                    model.meshes[meshIndex - 1].indexCount = (unsigned int)(&indices[verterIndex] - model.meshes[meshIndex - 1].indices);
                }
                model.meshes[meshIndex].material = ParseMTL((char *)mtlLibData.data, &current[7], memory);
                meshIndex++;
                current = endOfLine;
            }
            else if(*current == 'f' && current[1] == ' ')
            {
                for(int j = 0; j < indexCountPerLine; current++)
                {
                    if(*current == ' ')
                    {
                        int tempIndex = atoi(&current[1]);
                        if(tempIndex < 0)
                            tempIndex = posCount + tempIndex + 1;
                        vertices[verterIndex].position = position[tempIndex - 1];
                    }
                    else if(*current == '/' && (indexCountPerLine == 8 || indexCountPerLine == 4))
                        return Model();
                    else if(*current == '/' && indexCountPerLine == 6)
                    {
                        int tempIndex = atoi(&current[1]);
                        if(j % 2 == 0)
                        {
                            if(tempIndex < 0)
                                tempIndex = textureUVCount + tempIndex + 1;
                            vertices[verterIndex].textureUV = textureUV[tempIndex - 1];
                        }
                        if(j % 2 == 1)
                        {
                            if(tempIndex < 0)
                                tempIndex = normalCount + tempIndex + 1;
                            vertices[verterIndex++].normal = normal[tempIndex - 1];
                        }
                        j++;
                    }
                }
                if(indexCountPerLine == 0)
                {
                    current = SkipeWhiteSpace(current);
                    vertices[verterIndex++].position = position[atoi(current) - 1];
                    current = SkipeWhiteSpace(current);
                    vertices[verterIndex++].position = position[atoi(current) - 1];
                    current = SkipeWhiteSpace(current);
                    vertices[verterIndex++].position = position[atoi(current) - 1];
                    vertices[verterIndex - 1].normal = GetNormalFromTriangle(vertices[verterIndex - 3].position,
                    vertices[verterIndex - 2].position, vertices[verterIndex - 1].position);
                }
            }
            else
                current++;          
        }
    }
    if(materialsCount > 1)
    {
        model.meshes[meshIndex - 1].vertexCount = (unsigned int)(&indices[verterIndex] - model.meshes[meshIndex - 1].indices);
        model.meshes[meshIndex - 1].indexCount = (unsigned int)(&indices[verterIndex] - model.meshes[meshIndex - 1].indices);     
    }
    else
    {
        model.meshes[0].vertexCount = indexCount;
        model.meshes[0].indexCount = indexCount;
    }

    for (int i = 0; i < model.meshCount; i++)
    {
#if 1
        //Vertex Deduplication using a hashmap
        std::unordered_map <Vertex, int, Vertex::HashFunction> vertexMap;
        int removeSize = 0;
        for (int j = 0; j < (int)model.meshes[i].indexCount; j++)
        {
            if (vertexMap[model.meshes[i].vertices[j]] == 0)
                vertexMap[model.meshes[i].vertices[j]] = j - removeSize;
            else
                removeSize++;
            model.meshes[i].indices[j] = vertexMap[model.meshes[i].vertices[j]];
        }
        for (auto& it: vertexMap) {
            model.meshes[i].vertices[it.second] = it.first;
        }
        model.meshes[i].vertexCount = (unsigned int)vertexMap.size() + 1;
#endif
        //And generating the buffers after the deduplication
        GenerateBuffers(&model.meshes[i], 1);
    }

    TempMemoryEnd(tempMem);

    return model;
}
#endif


#if 0
Material ParseMTL(const char *buf, char* MaterialName, Memory* memory)
{
    Material result;;
    result.diffuseMap.index = 0;
    result.normalMap.index = 0;
    result.ambient = vec3(0.5f, 0.5f, 0.5f);
    result.diffuse = vec3(0.8f, 0.8f, 0.8f);
    result.specular = vec3(1, 1, 1);
    result.emissive = vec3(1, 1, 1);
    result.opaqueness = 1;
    result.shininess = 25;
    result.diffuseMap.index = 0;

    char *current = (char *)buf;
    char *endOfLine = (char *)buf;

    char  temp[255] = {};
    int i = 0;
    while (MaterialName[i] != '\n' && MaterialName[i] != ' ' && 
           MaterialName[i] != '\r')
    {
        temp[i] = MaterialName[i];
        i++;
    }
    temp[i] = '\0';
    while (*endOfLine != '\0')
    {
        current = endOfLine;
        endOfLine = GetNextLine(endOfLine);
        while (current != endOfLine && endOfLine > current)
        {
            if (*current == '#')
                current = endOfLine;
            if (strncmp(temp, current, strlen(temp)) == 0 && (current[strlen(temp)] == '\n' || current[strlen(temp)] == '\r'))
            {
                while (strncmp(current, "newmtl ", 7) != 0 && *current != '\0')
                {
                    if (strncmp(current, "norm ", 5) == 0)
					{
						i = 0;
                        while (current[i] != '\n' && current[i] != ' ' && current[i] != '\t' &&
                               current[i] != '\r')
                            i++;
                        i++;
                        int j = 0;
                        while (current[i] != '\n' && current[i] != ' ' && current[i] != '\t' &&
                               current[i] != '\r')
							temp[j++] = current[i++];
						temp[j] = '\0';
						result.normalMap = Texture(&temp[0]);
                        current += 5;
                    }
					else if (strncmp(current, "map_Kd ", 7) == 0)
					{
						i = 0;
                        while (current[i] != '\n' && current[i] != ' ' && current[i] != '\t' &&
                               current[i] != '\r')
                            i++;
                        i++;
                        int j = 0;
                        while (current[i] != '\n' && current[i] != ' ' && current[i] != '\t' &&
                               current[i] != '\r')
							temp[j++] = current[i++];
						temp[j] = '\0';
						result.diffuseMap = Texture(&temp[0]);
                        current += 7;
                    }
                    else if (strncmp(current, "Kd ", 3) == 0)
                    {
                        current = SkipeWhiteSpace(current);
                        result.diffuse = GetVec3(current);
                    }
                    else if (strncmp(current, "Ks ", 3) == 0)
                    {
                        current = SkipeWhiteSpace(current);
                        result.specular = GetVec3(current);
                    }
                    else if (strncmp(current, "Ke ", 3) == 0)
                    {
                        current = SkipeWhiteSpace(current);
                        result.emissive = GetVec3(current);
                    }
                    else if (strncmp(current, "d ", 2) == 0)
                    {
                        current = SkipeWhiteSpace(current);
                        result.opaqueness = (float)atof(current);
                    }
                    else if (strncmp(current, "Ns ", 3) == 0)
                    {
                        current = SkipeWhiteSpace(current);
                        result.shininess = (float)atof(current);
                    }
                    current++;
                }
                //memory.tempUsedSize = 0;
                return (result);
            }
            current++;
        }
    }
    return (result);
}

Model CreateObjVAO(const char *fileName, Memory* memory)
{
    Model model = {};

    model.modelMatrix = mat4();
    Material material = {};

    FILE *objFile = nullptr;
	fopen_s(&objFile, fileName, "rb");
    int sizeTotal = 0;
    char* buf = nullptr;
    if (objFile)
    {
        fseek(objFile, 0, SEEK_END);
        sizeTotal = ftell (objFile);
        fseek(objFile, 0, SEEK_SET);
        buf = (char *)MyMalloc(memory, sizeTotal);
        memset((void*)buf, 0, sizeTotal);
        fread((void*)buf, sizeTotal, 1, objFile);
    }
    else
        return (model);
    int IndexCountPerLine = -1;
    int PosCount       = 0;
    int NormalCount    = 0;
    int TextureUVCount = 0;
    int MaterialsCount = 0;
    int IndexCount     = 0;
    fclose(objFile);
    char *current = buf;
    char *endOfLine = buf;
    const char* mtlLib = nullptr;
    bool hasMtlFile = false;
    while (*endOfLine != '\0')
    {
        current = endOfLine;
        endOfLine = GetNextLine(current);

        while (current != endOfLine && endOfLine > current)
        {
            if (*current == '#')
                current = endOfLine;

            else if (*current == 'v' && current[1] == ' ')
            {
                current = SkipeWhiteSpace(current);
                PosCount++;
                current = endOfLine;
            }

            else if (*current == 'v' && current[1] == 't')
            {
                current = SkipeWhiteSpace(current);
                TextureUVCount++;
                current = endOfLine;
            }

            else if (*current == 'v' && current[1] == 'n')
            {
                current = SkipeWhiteSpace(current);
                NormalCount++;
                current = endOfLine;
            }

            else if (strncmp("mtllib", current, 6) == 0)
            {
                current = SkipeWhiteSpace(current);
                mtlLib  = current;
                current = endOfLine;
                hasMtlFile = true;
            }

            else if (((size_t)endOfLine - (size_t)current) >= 5 && strncmp("usemtl", current, 6) == 0)
            {
                MaterialsCount++;
                current = endOfLine;
            }

            else if (*current == 'f' && current[1] == ' ')
            {
                if (IndexCountPerLine == -1)
                {
                    IndexCountPerLine = 0;
                    for (int i = 0; current[i] != '\n'; i++)
                    {
                        if (current[i] == '/')
                            IndexCountPerLine++;
                    }
                }
                IndexCount += IndexCountPerLine / 2;
                current = endOfLine;
                if (IndexCountPerLine == 0)
                    IndexCount += 3;
            }

            else
                current++;
        }

    }

    int VertexIndex    = 0;
    int PositionIndex  = 0;
    int NormalIndex    = 0;
    int TextureUVIndex = 0;
    int meshIndex      = 0;
    if (!hasMtlFile)
        MaterialsCount = 1;

#if 0
    Mesh   *meshes    = (Mesh   *)MyMalloc   (memory, MaterialsCount * sizeof(Mesh));
    vec3   *position  = (vec3   *)MyTempAlloc(memory, PosCount       * sizeof(vec3));
    vec3   *normal    = (vec3   *)MyTempAlloc(memory, NormalCount    * sizeof(vec3));
    vec2   *textureUV = (vec2   *)MyTempAlloc(memory, TextureUVCount * sizeof(vec2));
    Vertex *vertices  = (Vertex *)MyTempAlloc(memory, IndexCount     * sizeof(Vertex));
    unsigned int    *indices   = (unsigned int    *)MyTempAlloc(memory, IndexCount     * sizeof(unsigned int));
#endif

    Mesh* meshes = (Mesh *)MyMalloc(memory, MaterialsCount * sizeof(Mesh));
    TempMemory tempMem = TempMemoryBegin(memory);
    vec3 *position = (vec3 *)MyMalloc(tempMem.memory, PosCount * sizeof(vec3));
    vec3 *normal = (vec3 *)MyMalloc(tempMem.memory, NormalCount * sizeof(vec3));
    vec2 *textureUV = (vec2 *)MyMalloc(tempMem.memory, TextureUVCount * sizeof(vec2));
    Vertex *vertices = (Vertex *)MyMalloc(tempMem.memory, IndexCount * sizeof(Vertex));
    unsigned int *indices = (unsigned int *)MyMalloc(tempMem.memory, IndexCount * sizeof(unsigned int *));
    char   MtlLibFilePath[255] = {};

    meshes[0].material = Material();
    strncpy_s(MtlLibFilePath, "../Assets/", 10); 

    int i = 0;
    while (MtlLibFilePath[i] != '\0')
        i++;
    int j = 0;
    if (hasMtlFile)
    {
        while (mtlLib[j] != '\n' && mtlLib[j] != '\t' && mtlLib[j] != '\r')
        {
            MtlLibFilePath[i] = mtlLib[j++];
            i++;
        }
        MtlLibFilePath[i] = '\0';
    }

    FileResource mtlLibData(MtlLibFilePath, memory);
    LoadFile(mtlLibData, memory);
    if (mtlLibData.file)
        fclose(mtlLibData.file);

    model.meshes = meshes;
    model.meshes[meshIndex].vertices = vertices;
    model.meshes[meshIndex].indices = indices;

    current = buf; 
    endOfLine = buf;
    model.meshCount = MaterialsCount;

    while (*endOfLine != '\0')
    {
        current = endOfLine;
        endOfLine = GetNextLine(current);
        while (current != endOfLine && endOfLine > current)
        {
            if (*current == '#')
            {
                current = endOfLine;
            }

            else if (*current == 'v' && current[1] == ' ')
            {
                current = SkipeWhiteSpace(current);
                position[PositionIndex] = GetVec3(current);
                PositionIndex++;
                current = endOfLine;
            }

            else if (*current == 'v' && current[1] == 't')
            {
                current = SkipeWhiteSpace(current);
                textureUV[TextureUVIndex] = GetVec2(current);
                TextureUVIndex++;
                current = endOfLine;
            }

            else if (*current == 'v' && current[1] == 'n')
            {
                current = SkipeWhiteSpace(current);
                normal[NormalIndex] = GetVec3(current);
                NormalIndex++;
                current = endOfLine;
            }

            else if (strncmp("mtllib", current, 6) == 0)
            {
                current = endOfLine;
            }

            else if (((size_t)endOfLine - (size_t)current) >= 5 && strncmp("usemtl", current, 6) == 0)
            {
                model.meshes[meshIndex].indices = &indices[VertexIndex];
                model.meshes[meshIndex].vertices = &vertices[VertexIndex];
                if (meshIndex >= 1)
                {
                    model.meshes[meshIndex - 1].vertexCount = (unsigned int)(&indices[VertexIndex] - model.meshes[meshIndex - 1].indices);
                    model.meshes[meshIndex - 1].indexCount = (unsigned int)(&indices[VertexIndex] - model.meshes[meshIndex - 1].indices);
                }
                model.meshes[meshIndex].material = ParseMTL((char *)mtlLibData.data, &current[7], memory);
                meshIndex++;
                current = endOfLine;
            }

            else if (*current == 'f' && current[1] == ' ')
            {
                for(int j = 0; j < IndexCountPerLine; current++)
                {
                    if (*current == ' ')
                    {
                        int TempIndex = atoi(&current[1]);
                        if (TempIndex < 0)
                            TempIndex = PosCount + TempIndex + 1;
                        vertices[VertexIndex].position = position[TempIndex - 1];
                    }
                    else if (*current == '/' && (IndexCountPerLine == 8 || IndexCountPerLine == 4))
                        return(Model());
                    else if (*current == '/' && IndexCountPerLine == 6)
                    {
                        int TempIndex = atoi(&current[1]);
                        if (j % 2 == 0)
                        {
                            if (TempIndex < 0)
                                TempIndex = TextureUVCount + TempIndex + 1;
                            vertices[VertexIndex].textureUV = textureUV[TempIndex - 1];
                        }
                        if (j % 2 == 1)
                        {
                            if (TempIndex < 0)
                                TempIndex = NormalCount + TempIndex + 1;
                            vertices[VertexIndex++].normal = normal[TempIndex - 1];

                        }
                        j++;
                    }
                }
                if (IndexCountPerLine == 0)
                {
                    current = SkipeWhiteSpace(current);
                    vertices[VertexIndex++].position = position[atoi(current) - 1];
                    current = SkipeWhiteSpace(current);
                    vertices[VertexIndex++].position = position[atoi(current) - 1];
                    current = SkipeWhiteSpace(current);
                    vertices[VertexIndex++].position = position[atoi(current) - 1];
                    vertices[VertexIndex - 1].normal = GetNormalFromTriangle(vertices[VertexIndex - 3].position,
                                                                               vertices[VertexIndex - 2].position,
                                                                               vertices[VertexIndex - 1].position);
                }
            }
            else
                current++;
        }
    }
    if (MaterialsCount > 1)
    {
        model.meshes[meshIndex - 1].vertexCount = (unsigned int)(&indices[VertexIndex] - model.meshes[meshIndex - 1].indices);
        model.meshes[meshIndex - 1].indexCount = (unsigned int)(&indices[VertexIndex] - model.meshes[meshIndex - 1].indices);
    }
    else
    {
        model.meshes[0].vertexCount = IndexCount;
        model.meshes[0].indexCount = IndexCount;
    }
    for (int i = 0; i < model.meshCount; i++)
    {
        //Vertex Deduplication using a hashmap
        std::unordered_map <Vertex, int, Vertex::HashFunction> vertexMap;
        int removeSize = 0;
        for (int j = 0; j < (int)model.meshes[i].indexCount; j++)
        {
            if (vertexMap[model.meshes[i].vertices[j]] == 0)
                vertexMap[model.meshes[i].vertices[j]] = j - removeSize;
            else
                removeSize++;
            model.meshes[i].indices[j] = vertexMap[model.meshes[i].vertices[j]];
        }
        for (auto& it: vertexMap) {
            model.meshes[i].vertices[it.second] = it.first;
        }
        model.meshes[i].vertexCount = (unsigned int)vertexMap.size() + 1;
        //And generating the buffers after the deduplication
        GenerateBuffers(&model.meshes[i], 1);
    }

    TempMemoryEnd(tempMem);

    return (model);
}

#endif