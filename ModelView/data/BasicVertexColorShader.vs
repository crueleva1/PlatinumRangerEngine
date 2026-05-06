#version 410

uniform mat4 uWorldMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;

layout (location = 0) in vec3 iVertices;
layout (location = 1) in vec2 iTexture1;
layout (location = 2) in vec4 iVertexColor1;

out vec2 voTextureCoord;
out vec4 voVertexColor;

void main()
{
    mat4 kWVPMat = uProjectionMatrix * uViewMatrix * uWorldMatrix;
    voTextureCoord = iTexture1;
    voVertexColor = iVertexColor1;
    gl_Position = kWVPMat * vec4(iVertices.xyz, 1.0);
}