#version 330

in vec2 TexCoord;

uniform sampler2D uDiffuseMap;
vec4 FragColor;

uniform bool isColor;
uniform vec3 Color;

void main()
{
    FragColor = texture(uDiffuseMap, TexCoord);
}