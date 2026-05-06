#version 410
 
uniform vec4 uColor;

layout (location = 0) out vec4 oColor;
layout (location = 1) out vec4 oBrightness;
 
void main()
{
    oColor = uColor;
    oBrightness = uColor;
}