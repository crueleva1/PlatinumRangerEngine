#version 410

uniform mat4 uWorldMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;
uniform sampler2D uHeight;

in vec3 iVertices;
in vec3 iNormals;
in vec2 iTexture1;

out vec2 voTextureCoord;

void main()
{
    mat4 kWVPMat = uProjectionMatrix * uViewMatrix * uWorldMatrix;
    voTextureCoord = iTexture1;
    voTextureCoord.y = 1.0 - voTextureCoord.y;
    vec4 kHeightedVertex = vec4(iVertices, 1.0);
    kHeightedVertex.y += (texture2D(uHeight, voTextureCoord).r * 5.0);
    gl_Position = kWVPMat * kHeightedVertex;
}