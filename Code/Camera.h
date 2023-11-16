#pragma once
#include "Matrices.h"

struct Camera
{
    mat4 viewMatrix;
    vec3 position;
    vec3 targetPosition;

    vec3 forward;
    vec3 right;
    vec3 up;
};


#define UPDATE_CAMERA(name) void name(Camera* camera,GLFWwindow* window ,float deltaTime)
//typedef UPDATE_CAMERA()