#version 410

uniform sampler2D uDiffuse;

in vec2 voTextureCoord;
in vec4 voVertexColor;

layout (location = 0) out vec4 oColor;

void main()
{
    oColor = voVertexColor * texture(uDiffuse, voTextureCoord);
}