#version 410

uniform sampler2D uOpacity;
uniform vec3 uViewWorldPos;
uniform bool uHasMask;

in vec3 voWorldPos;
in vec2 voTextureCoord;

layout (location = 0) out vec4 oColor;

void main()
{
    float fAlpha = 1.0;
    if (uHasMask)
        fAlpha = texture2D(uOpacity, voTextureCoord).r;

    if (fAlpha < 1.0)
        discard;
    oColor.r = length(voWorldPos - uViewWorldPos);
}