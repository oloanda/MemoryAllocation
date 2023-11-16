#version 330

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexture;


uniform mat4 uWorldTransform;
uniform mat4 uViewProj;

out vec3 Normal;
out vec2 TexCoord;
out vec3 FragPos;

void main()
{
    vec4 pos =  vec4(inPosition, 1.0);
    pos = pos * uWorldTransform;
    FragPos = pos.xyz;

    Normal = inNormal * mat3(transpose(inverse(uWorldTransform)));

    gl_Position = pos * uViewProj;
    TexCoord = inTexture;
}