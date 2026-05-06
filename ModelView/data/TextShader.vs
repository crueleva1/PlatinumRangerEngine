#version 410

uniform mat4 uWorldMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;

in vec3 iVertices;
in vec3 iNormals;
in vec2 iTexture1;

out vec2 voTextureCoord;

void main()
{
    mat4 kWVPMat = uProjectionMatrix * uWorldMatrix;
    gl_Position = kWVPMat * vec4(iVertices, 1.0);
    voTextureCoord = iTexture1;
    voTextureCoord.y = 1.0 - voTextureCoord.y;
}