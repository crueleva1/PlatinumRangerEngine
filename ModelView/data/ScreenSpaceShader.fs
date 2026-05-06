#version 410

uniform sampler2D uDiffuse;

in vec2 voTextureCoord;

out vec4 oColor;

void main()
{
    oColor = texture2D(uDiffuse, voTextureCoord);
}