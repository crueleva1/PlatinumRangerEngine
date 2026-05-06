#version 410

uniform mat4 uWorldMatrix;
uniform mat4 uViewMatrix;
uniform mat4 uProjectionMatrix;

in vec3 iVertices;
in vec2 iTexture1;

out vec4 voPosition;

void main()
{
    mat4 kWVPMat = uProjectionMatrix * uViewMatrix * uWorldMatrix;
    gl_Position = kWVPMat * vec4(iVertices, 1.0);
    voPosition = gl_Position;
}