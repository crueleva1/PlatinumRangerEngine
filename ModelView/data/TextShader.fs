#version 410

uniform sampler2D uDiffuse;
uniform vec4 uTextColor;

in vec2 voTextureCoord;

out vec4 oColor;

void main()
{
    oColor = texture2D(uDiffuse, voTextureCoord).rrrr;
    //oColor = oColor * uTextColor;
}