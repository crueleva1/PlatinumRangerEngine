#version 410

uniform sampler2D uDiffuse;
uniform sampler2D uOpacity;
uniform bool uHasMask;

in vec2 voTextureCoord;

out vec4 oColor;

void main()
{
    oColor = texture2D(uDiffuse, voTextureCoord);
    if (uHasMask) {
        oColor.a = texture2D(uOpacity, voTextureCoord).r;
    }
}