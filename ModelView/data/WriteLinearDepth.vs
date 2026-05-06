#version 410

uniform mat4 uWorldMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;

in vec3 iVertices;
in vec2 iTexture1;

out vec3 voWorldPos;
out vec2 voTextureCoord;

void main()
{
    mat4 kWVPMat = uProjectionMatrix * uViewMatrix * uWorldMatrix;

    voTextureCoord = iTexture1;
    voTextureCoord.y = 1.0 - voTextureCoord.y;
    vec4 kVertex = vec4(iVertices, 1.0);
    gl_Position = kWVPMat * kVertex;
    
    voWorldPos = (uWorldMatrix * kVertex).xyz;
    voTextureCoord = voTextureCoord;
}