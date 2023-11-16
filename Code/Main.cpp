#define _CRT_SECURE_NO_WARNINGS
#include "glad/glad.h"
#include "glfw3.h"



#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "ResourceManager.h"
#include "Parser.h"
#include "Shader.h"

#include "Texture.h"
#include "Matrices.h"
#include "Quat.h"

#include "Model.h"
#include "Sprite.h"
#include "Camera.h"


void Resize(GLFWwindow* window);
void ProcessInput(GLFWwindow* window, float deltaTime);

int gScreenWidth = 1024;
int gScreenHeight = 768;

//-0.4 10 63
//vec3 cameraPos(0.0f, 5.0f, 20.0f);
vec3 cameraPos(-0.4f, 10.0f, 63.0f);
vec3 cameraForward(0.0f, 0.0f, -1.0f);
vec3 cameraUp(0.0f, 1.0f, 0.0f);

vec3 lightPos(0.0f, 1.2f, 1.0f);

float lastFrame = 0.0f;
int frameCount = 0;
int noLights = 0;


UPDATE_CAMERA(UpdateCamera)
{
    //camera->
}

unsigned int CompileSpriteShader(Memory* memory)
{
    unsigned int shaderID = 0;
    Shader vertexShader("../Shaders/Sprite.vert",GL_VERTEX_SHADER, memory);
    Shader fragmentShader("../Shaders/Sprite.frag",GL_FRAGMENT_SHADER, memory);


    shaderID = glCreateProgram();
    glAttachShader(shaderID, vertexShader.index);
    glAttachShader(shaderID, fragmentShader.index);
    glLinkProgram(shaderID);

    int success;
    char infoLog[512] = {0};
    glGetProgramiv(shaderID, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(shaderID, 512, NULL, infoLog);
    }

    glDeleteShader(vertexShader.index);
    glDeleteShader(fragmentShader.index);

    return shaderID;
}

unsigned int CompileShader(Memory* memory)
{
    unsigned int shaderID = 0;
    Shader vertexShader("../Shaders/Basic.vert",GL_VERTEX_SHADER, memory);
    Shader fragmentShader("../Shaders/Basic.frag",GL_FRAGMENT_SHADER, memory);
#if 0
    Shader vertexShader("../Shaders/BasicMesh.vert",GL_VERTEX_SHADER, memory);
    Shader fragmentShader("../Shaders/BasicMesh.frag",GL_FRAGMENT_SHADER, memory);
#endif

    shaderID = glCreateProgram();
    glAttachShader(shaderID, vertexShader.index);
    glAttachShader(shaderID, fragmentShader.index);
    glLinkProgram(shaderID);

    int success;
    char infoLog[512] = {0};
    glGetProgramiv(shaderID, GL_LINK_STATUS, &success);
    if(!success)
    {
        glGetProgramInfoLog(shaderID, 512, NULL, infoLog);
    }

    glDeleteShader(vertexShader.index);
    glDeleteShader(fragmentShader.index);

    return shaderID;
}

static void SetInt(unsigned int& shader,const char* name, int value)
{
    glUniform1i(glGetUniformLocation(shader, name), value);
}

static void SetFloat(unsigned int& shader, const char* name, float value)
{
    glUniform1f(glGetUniformLocation(shader, name),value);
}

static void SetVec3(unsigned int& shader, const char* name, const vec3& vec)
{
    glUniform3f(glGetUniformLocation(shader, name), vec.x, vec.y, vec.z);
}

static void SetMatrix(unsigned int& shader, const char* name, const mat4& matrix)
{
    glUniformMatrix4fv(glGetUniformLocation(shader, name),1, GL_TRUE, matrix.asArray);
}

void SetLights(unsigned int& shader)
{
    vec3 lightColor(1.0f, 1.0f, 1.0f);
#if 0
    lightColor.r = (float)sin(glfwGetTime() * 2.0) * 0.5f + 0.5f;
    lightColor.g = (float)sin(glfwGetTime() * 0.7) * 0.5f + 0.5f;
    lightColor.b = (float)sin(glfwGetTime() * 1.3) * 0.5f + 0.5f;
#endif
    vec3 diffuseColor =  vec3(0.96f, 0.96f, 0.96f) * lightColor;
    vec3 ambientColor =  vec3(0.5f, 0.5f, 0.5f)    * diffuseColor;

    SetVec3(shader,"uLight.position",lightPos);
    SetVec3(shader,"uLight.ambient",ambientColor);
    SetVec3(shader,"uLight.diffuse",diffuseColor);
    SetVec3(shader,"uLight.specular", vec3(1.0f, 1.0f, 1.0f));

    SetFloat(shader,"uLight.constant", 1.0f);
    SetFloat(shader,"uLight.linear", 0.14f);
    SetFloat(shader,"uLight.quadratic", 0.07f);

    SetInt(shader,"noLights",noLights);

}

void DrawSprite(Sprite& sprite, unsigned int& shader)
{
    glUseProgram(shader);
    glBindVertexArray(sprite.VAO);

    mat4 viewProj = SimpleViewProj(1024.0f, 768.0f);
    mat4 scale = Scale(sprite.diffuseMap.width, sprite.diffuseMap.height,1);

    //sprite.modelMatrix = scale * sprite.modelMatrix;
    SetMatrix(shader, "uViewProj", viewProj);
    SetMatrix(shader, "uWorldTransform", sprite.modelMatrix);

    //glUniform1i(glGetUniformLocation(shader,"uDiffuseMap"), 0);

    glBindTexture(GL_TEXTURE_2D, sprite.diffuseMap.index);  

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

}

void DrawModel(Model& model, unsigned int& shader, const mat4& viewProj)
{

    for(int i = 0; i < model.meshCount; i++)
    {
        glUseProgram(shader);
        glBindVertexArray(model.meshes[i].VAO);
        SetMatrix(shader, "uViewProj", viewProj);
        SetMatrix(shader, "uWorldTransform", model.modelMatrix);

        SetVec3(shader, "uViewPos", cameraPos);
        if(model.meshes[i].material.normalMap.index == 0)
        {
            glUniform1i(glGetUniformLocation(shader,"hasNormalMap"), 0);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        else
        {
            glUniform1i(glGetUniformLocation(shader,"hasNormalMap"), 1);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, model.meshes[i].material.normalMap.index);      
        }

        glUniform1i(glGetUniformLocation(shader,"uSpecularMap"), 2);
        if(model.meshes[i].material.specularMap.index == 0)
        {
            glUniform1i(glGetUniformLocation(shader,"hasSpecularMap"), 0);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        else
        {
            glUniform1i(glGetUniformLocation(shader,"hasSpecularMap"), 1);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, model.meshes[i].material.specularMap.index);      
        }
        SetVec3(shader,"uMaterial.ambient", vec3(model.meshes[i].material.ambient));
        SetVec3(shader,"uMaterial.diffuse", vec3(model.meshes[i].material.diffuse));
        SetVec3(shader,"uMaterial.specular", vec3(model.meshes[i].material.specular));
        SetVec3(shader,"uMaterial.emissive", vec3(model.meshes[i].material.emissive));
        SetFloat(shader,"uMaterial.shininess", model.meshes[i].material.shininess);

        glUniform1i(glGetUniformLocation(shader,"uDiffuseMap"), 0);
        if(model.meshes[i].material.diffuseMap.index == 0)
        {
            glUniform1i(glGetUniformLocation(shader,"isColor"), 1);
        }
        else
        {
            glUniform1i(glGetUniformLocation(shader,"isColor"), 0);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, model.meshes[i].material.diffuseMap.index);  
        }

        glDrawElements(GL_TRIANGLES, model.meshes[i].indexCount, GL_UNSIGNED_INT, nullptr);
        //glDrawArrays(GL_TRIANGLES, 0,model.meshes[i].vertexCount);
        
    }
}

int main(int argc, char** argv)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, true);
    //glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, 1);

    GLFWwindow* Window = glfwCreateWindow(gScreenWidth, gScreenHeight, "Game Programming", NULL, NULL);
    if(!Window)
    {
        printf("Failed to create GLFW wiindow !!\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(Window);


    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("Failed to initialize GLAD !!!\n");
        return -1;
    }
  

    Memory memory = {};
    memory = InitMemory(7LL * GigaByte);

    unsigned int shaderProgram = 0;
    shaderProgram = CompileShader(&memory);

    unsigned int spriteShader = 0;
    spriteShader = CompileSpriteShader(&memory);

    


    //Model floor = CreateObjVAO("../Assets/WalledCity.obj", &memory);
	Model floor = CreateObjVAO("../Assets/floor.obj", &memory);
    floor.modelMatrix = Scale(vec3(100.0f, 1.0f, 100.0f));
    floor.modelMatrix = floor.modelMatrix * Translation(vec3(0.0f, 0.0f, 0.0f));
#if 1
    Model objModel = CreateObjVAO("../Assets/robot.obj", &memory);
    objModel.modelMatrix = Scale(vec3(2.5f, 2.5f, 2.5f));
    objModel.modelMatrix = objModel.modelMatrix * Translation(vec3(15.5f, 0.0f, -4.0f));

    Model SpiderMan = CreateObjVAO("../Assets/Spider-Man.obj", &memory);
    SpiderMan.modelMatrix = Scale(vec3(0.09f, 0.09f, 0.09f));
    SpiderMan.modelMatrix = SpiderMan.modelMatrix * Translation(vec3(-15.5f, 0.0f, -4.0f));


    Model Alduin = CreateObjVAO("../Assets/Alduin.obj", &memory);
    Model bunny = CreateObjVAO("../Assets/bunny.obj", &memory);

    //Model Kelial  = CreateObjVAO("../Assets/SB_PC_BOSS_Kelial.obj", &memory);

#endif

    Sprite sprite = CreateSprite("../Assets/airplane.png");
    //sprite.modelMatrix = Translation(vec3(400,200, 0));


    while(!glfwWindowShouldClose(Window))
    {

        float currFrame = (float)glfwGetTime();
        float deltaTime = currFrame - lastFrame;
        lastFrame = currFrame;
        frameCount++;

        static float angle  = 0.0f;
        angle += 50.0f * deltaTime;

        //UpdateCamera(&camera,Window ,deltaTime);


#if 1
        Alduin.modelMatrix = Scale(vec3(0.03f, 0.03f, 0.03f));
        Alduin.modelMatrix = Alduin.modelMatrix * QuatToMat4(AngleAxis(PiOver2 , vec3(0.0f, 1.0f, 0.0f))); 
        Alduin.modelMatrix = Alduin.modelMatrix * Translation(vec3(0.0f, 15.0f, -2.5f + 10.0f * sinf(DEG2RAD(angle))));

        bunny.modelMatrix = Scale(vec3(4.5f, 4.5f, 4.5f));
        bunny.modelMatrix = bunny.modelMatrix * QuatToMat4(AngleAxis(DEG2RAD(-  angle), vec3(0.0f, 1.0f, 0.0f))); 
        bunny.modelMatrix = bunny.modelMatrix * Translation(vec3(0.0f, 0.0f, -5.0f));
#endif
        float x = 3.0f * sinf(DEG2RAD(angle));
        float z = 1.5f + 1.5f * cosf(DEG2RAD(angle));-
        lightPos = vec3(x, 0.0f, z);
        lightPos.y += 3.8f;



        glfwPollEvents();
        ProcessInput(Window, deltaTime);

        glClearColor(0.23f, 0.33f, 0.47f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        Resize(Window);

        mat4 view = mat4();
        mat4 proj = mat4();

        view = LookAt(cameraPos, cameraPos + cameraForward * 100, cameraUp);
        proj = Projection(45.0f, (float)gScreenWidth / (float)gScreenHeight, 0.1f, 100000.0f);

        glUseProgram(shaderProgram);
        SetLights(shaderProgram);
        SetVec3(shaderProgram, "uViewPos", cameraPos);

#if 1
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        DrawModel(floor,shaderProgram, view * proj);
        DrawModel(objModel,shaderProgram, view * proj);
        DrawModel(Alduin,shaderProgram, view * proj);
        DrawModel(SpiderMan,shaderProgram, view * proj);
		
        //DrawModel(Kelial,shaderProgram, view * proj);
        if((frameCount / 120) % 2) DrawModel(bunny,shaderProgram, view * proj);
 
#endif
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_BLEND);
       	glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);
	    glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);

        glUseProgram(spriteShader);
        DrawSprite(sprite, spriteShader);


        glfwSwapBuffers(Window);
    }



    glfwTerminate();

    return 0;
}


void ProcessInput(GLFWwindow* window, float deltaTime)
{
    if(glfwGetKey(window,GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);


  float cameraSpeed = 15.0f * deltaTime;
  if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
      cameraPos += cameraForward * cameraSpeed;
  if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
      cameraPos -= cameraForward * cameraSpeed;
      
  if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    cameraPos -= Normalized(Cross(cameraForward, cameraUp)) * cameraSpeed;
  if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    cameraPos += Normalized(Cross(cameraForward, cameraUp)) * cameraSpeed;
  if(glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
    cameraPos -= cameraUp * cameraSpeed;
  if(glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
    cameraPos += cameraUp * cameraSpeed;

    if(glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
        noLights = !noLights;
    //if(glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        //glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);

}

void Resize(GLFWwindow* window)
{
    int width,height;
    glfwGetWindowSize(window,&width ,&height);
    glViewport(0, 0, width, height);
}