#version 150


uniform mat4 uProjectionMatrix;

in vec3 iVertices;
in vec2 iTexture1;

out vec2 voTextureCoord;


void main()
{
    mat4 kWVPMat = uProjectionMatrix;
    gl_Position = kWVPMat * vec4(iVertices, 1.0);
    voTextureCoord = iTexture1;
}


