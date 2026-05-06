#version 410

uniform mat4 uWorldMatrix;
uniform mat4 uViewProjMatrix;

in vec3 iVertices;
in vec3 iNormals;
in vec3 iTangents;
in vec3 iBiNormals;
in vec2 iTexture1;

out vec4 voWorldPosition;
out vec3 voNormal;
out vec3 voTangent;
out vec2 voTextureCoord;

void main()
{
    mat4 kWVPMat = uViewProjMatrix * uWorldMatrix;

    voTextureCoord = iTexture1;
    voTextureCoord.y = 1.0 - voTextureCoord.y;
    vec4 kVertex = vec4(iVertices, 1.0);
    gl_Position = kWVPMat * kVertex;
    
    voWorldPosition = uWorldMatrix * kVertex;

    // Normal in world space
    mat3 kNormalMat = transpose(inverse(mat3(uWorldMatrix)));
    voNormal = kNormalMat * normalize(iNormals);
    voTangent = kNormalMat * normalize(iTangents);
}